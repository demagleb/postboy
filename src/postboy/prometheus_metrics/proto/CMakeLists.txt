generate_proto_cpp(
    proto_metrics_format

    import_metrics.proto
    metric_family.proto

    ################### metrics ###################

    metrics/labels.proto
    metrics/counter.proto
    metrics/gauge.proto
)
