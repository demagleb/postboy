generate_proto_cpp(
    config_proto

    binding.proto
    config.proto
    postboy.proto
    process.proto
    prometheus.proto
    telegram_notifier.proto
    thread.proto

    ################ processors utils ################

    processors/utils/binance.proto
    processors/utils/connection_pull.proto

    ################### processors ###################

    processors/binance_candle_generator.proto
    processors/binance_feature_generator.proto
    processors/binance_md_connector.proto
    processors/binance_millitant_strategy.proto
    processors/common.proto
    processors/benchmark.proto
    processors/instrument_downloader.proto
    processors/lgbm_aggressive_predictor.proto
    processors/rinat_strategy.proto
    processors/websocket.proto
)
