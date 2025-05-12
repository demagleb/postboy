#include "text_serializer.hpp"

#include <common/util/types.hpp>

#include <cmath>
#include <limits>
#include <locale>
#include <ostream>
#include <string>

namespace postboy::prometheus {

namespace {

FORCE_INLINE void WriteValue(std::ostream& out, double value) {
    if (std::isnan(value)) {
        out << "Nan";
    } else if (std::isinf(value)) {
        out << (value < 0 ? "-Inf" : "+Inf");
    } else {
        out << value;
    }
}

FORCE_INLINE void WriteValue(std::ostream& out, const std::string& value) {
    for (auto c : value) {
        switch (c) {
            case '\n':
                out << '\\' << 'n';
                break;

            case '\\':
                out << '\\' << c;
                break;

            case '"':
                out << '\\' << c;
                break;

            default:
                out << c;
                break;
        }
    }
}

template <typename T = std::string>
FORCE_INLINE void WriteHead(std::ostream& out, const MetricFamily& family, const ClientMetric& metric,
                            const std::string& suffix = "", const std::string& extraLabelName = "",
                            const T& extraLabelValue = T()) {
    out << family.name << suffix;
    if (!metric.label.empty() || !extraLabelName.empty()) {
        out << "{";
        const char* prefix = "";

        for (auto& lp : metric.label) {
            out << prefix << lp.name << "=\"";
            WriteValue(out, lp.value);
            out << "\"";
            prefix = ",";
        }
        if (!extraLabelName.empty()) {
            out << prefix << extraLabelName << "=\"";
            WriteValue(out, extraLabelValue);
            out << "\"";
        }
        out << "}";
    }
    out << " ";
}

FORCE_INLINE void WriteTail(std::ostream& out, const ClientMetric& metric) {
    if (LIKELY(metric.timestamp_ms != 0)) {
        out << " " << metric.timestamp_ms;
    }
    out << "\n";
}

void SerializeCounter(std::ostream& out, const MetricFamily& family, const ClientMetric& metric) {
    WriteHead(out, family, metric);
    WriteValue(out, metric.counter.value);
    WriteTail(out, metric);
}

void SerializeGauge(std::ostream& out, const MetricFamily& family, const ClientMetric& metric) {
    WriteHead(out, family, metric);
    WriteValue(out, metric.gauge.value);
    WriteTail(out, metric);
}

void SerializeHistogram(std::ostream& out, const MetricFamily& family, const ClientMetric& metric) {
    auto& hist = metric.histogram;
    WriteHead(out, family, metric, "_count");
    out << hist.sample_count;
    WriteTail(out, metric);

    WriteHead(out, family, metric, "_sum");
    WriteValue(out, hist.sample_sum);
    WriteTail(out, metric);

    double last = -std::numeric_limits<double>::infinity();
    for (auto& b : hist.buckets) {
        WriteHead(out, family, metric, "_bucket", "le", b.upper_bound);
        last = b.upper_bound;
        out << b.cumulative_count;
        WriteTail(out, metric);
    }

    if (last != std::numeric_limits<double>::infinity()) {
        WriteHead(out, family, metric, "_bucket", "le", "+Inf");
        out << hist.sample_count;
        WriteTail(out, metric);
    }
}

void SerializeFamily(std::ostream& out, const MetricFamily& family) {
    if (!family.help.empty()) {
        out << "# HELP " << family.name << " " << family.help << "\n";
    }
    switch (family.type) {
        case MetricType::Counter: {
            out << "# TYPE " << family.name << " counter\n";
            for (auto& metric : family.metric) {
                SerializeCounter(out, family, metric);
            }
            break;
        }
        case MetricType::Gauge: {
            out << "# TYPE " << family.name << " gauge\n";
            for (auto& metric : family.metric) {
                SerializeGauge(out, family, metric);
            }
            break;
        }
        case MetricType::Histogram: {
            out << "# TYPE " << family.name << " histogram\n";
            for (auto& metric : family.metric) {
                SerializeHistogram(out, family, metric);
            }
            break;
        }
    }
}

} // namespace

TextSerializer::TextSerializer() {
    out_.imbue(std::locale::classic());
    out_.precision(std::numeric_limits<double>::max_digits10 - 1);
}

std::string TextSerializer::Serialize(const std::vector<MetricFamily>& metrics) const {
    out_.str(std::string());
    Serialize(out_, metrics);
    return out_.str();
}

void TextSerializer::Serialize(std::ostream& out, const std::vector<MetricFamily>& metrics) const {
    for (auto& family : metrics) {
        SerializeFamily(out, family);
    }
}

} // namespace postboy::prometheus
