load("@rules_java//java:defs.bzl", "java_binary")

java_binary(
    name = "AvroTour",
    srcs = ["AvroTour.java"],
    main_class = "AvroTour",
    deps = [
        "@maven//:org_apache_avro_avro",
    ],
    runtime_deps = [
        # Avro logs through slf4j; a provider avoids the noisy
        # "SLF4J: No SLF4J providers were found" warning at runtime.
        "@maven//:org_slf4j_slf4j_simple",
    ],
)
