// test.java — 18 years of Java language change, executable.
// Compile: ./compile.sh    Run: java Tour   (or: java test.java — single-file source launch, Java 11)

import java.io.BufferedReader;
import java.io.IOException;
import java.io.StringReader;
import java.time.LocalDate;
import java.time.temporal.ChronoUnit;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.SequencedCollection;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.stream.Collectors;
import java.util.stream.Stream;

class Tour {

    public static void main(String[] args) throws Exception {
        java7();
        java8();
        java9to11();
        java12to17();
        java21();
        beyond21();
    }

    // ---------- Java 7 (2011): Project Coin ----------
    static void java7() throws IOException {
        banner("Java 7 (2011) — small change, after five frozen years");

        Map<String, List<Integer>> m = new LinkedHashMap<>();      // diamond <>
        m.put("primes", Arrays.asList(2, 3, 5));
        int million = 1_000_000, mask = 0b1010_1010;               // literals

        String env = "staging";
        switch (env) {                                              // strings in switch
            case "prod": case "staging": System.out.println("  env recognised: " + env); break;
            default: System.out.println("  unknown env");
        }

        try (BufferedReader r = new BufferedReader(new StringReader("first\nsecond"))) {
            System.out.println("  try-with-resources: " + r.readLine());
        } catch (IOException | RuntimeException e) {                // multi-catch
            System.out.println("  multi-catch: " + e);
        }
        System.out.println("  literals: " + million + " / " + mask + " / " + m);
    }

    // ---------- Java 8 (2014): the inflection ----------
    interface Named {
        String name();
        default String greet() { return "hello, " + name(); }       // 8: default method
        static Named of(String n) { return () -> n; }               // 8: static method + lambda
        private String tag() { return "[" + name() + "]"; }         // 9: private interface method
        default String tagged() { return tag() + " " + greet(); }
    }

    static void java8() {
        banner("Java 8 (2014) — lambdas, streams, Optional, java.time");

        List<String> shards = Arrays.asList("shard-3", "shard-1", "shard-2");
        System.out.println("  stream: " + shards.stream().sorted()
                .map(String::toUpperCase).collect(Collectors.joining(", ")));

        Optional<String> hit = shards.stream().filter(s -> s.endsWith("1")).findFirst();
        System.out.println("  optional: " + hit.orElse("none"));

        System.out.println("  default+static+private iface: " + Named.of("broker").tagged());

        System.out.println("  CompletableFuture: " + CompletableFuture
                .supplyAsync(() -> "computed").thenApply(String::toUpperCase).join());

        long years = ChronoUnit.YEARS.between(LocalDate.of(2014, 3, 18), LocalDate.now());
        System.out.println("  java.time: Java 8 is " + years + " years old today");
    }

    // ---------- Java 9-11 ----------
    static void java9to11() {
        banner("Java 9-11 (2017-18) — factories, var, String, six-month cadence");

        List<Integer> nums = List.of(1, 2, 3, 4, 5, 99, 6);          // 9: immutable factories
        System.out.println("  takeWhile: " + nums.stream().takeWhile(n -> n < 10).toList());
        Optional.of("x").ifPresentOrElse(                            // 9
                v -> System.out.println("  ifPresentOrElse: " + v), () -> {});

        var greeting = "  var + String methods: ";                   // 10: local type inference
        System.out.println(greeting + "'" + "  padded  ".strip() + "' "  // 11
                + "-".repeat(8) + " blank=" + "   ".isBlank());
    }

    // ---------- Java 12-17: preview machine ----------
    sealed interface Shape permits Circle, Square, Rect {}           // 17: sealed
    record Circle(double r) implements Shape {}                      // 16: records
    record Square(double side) implements Shape {}
    record Rect(double w, double h) implements Shape {}

    static void java12to17() {
        banner("Java 12-17 (2019-21) — switch expressions, text blocks, records, sealed");

        int shards = 3;
        String tier = switch (shards) {                              // 14: switch expression
            case 1, 2 -> "small";
            case 3 -> { yield "medium"; }                            // 14: yield
            default -> "large";
        };
        System.out.println("  switch expression: " + tier);

        String json = """
                {
                  "engine": "scripty",
                  "shards": %d
                }""".formatted(shards);                              // 15: text block
        System.out.println("  text block:\n" + json.indent(4).stripTrailing());

        Object o = new Circle(2.0);
        if (o instanceof Circle c && c.r() > 1) {                    // 16: instanceof pattern
            System.out.println("  instanceof pattern: radius " + c.r());
        }
        System.out.println("  record equals/toString for free: " + new Circle(2.0).equals(o));

        try {
            Map<String, int[]> m = new HashMap<>();
            int bad = m.get("missing")[0];                           // 14: helpful NPE
            System.out.println(bad);
        } catch (NullPointerException e) {
            System.out.println("  helpful NPE: " + e.getMessage());
        }
    }

    // ---------- Java 21: the second inflection ----------
    static String describe(Shape s) {
        return switch (s) {                                          // 21: pattern matching + records
            case Circle c when c.r() > 10 -> "big circle";           // 21: guarded pattern
            case Circle(double r) -> "circle r=" + r;                // 21: record pattern
            case Square(double side) -> "square side=" + side;
            case Rect(double w, double h) when w == h -> "square-ish rect";
            case Rect(double w, double h) -> "rect " + w + "x" + h;
        };                                                           // exhaustive: sealed, no default
    }

    static void java21() throws Exception {
        banner("Java 21 LTS (2023) — patterns, sequenced collections, virtual threads");

        Stream.of(new Circle(2), new Circle(99), new Square(3), new Rect(4, 4), new Rect(1, 2))
                .map(Tour::describe).forEach(d -> System.out.println("  " + d));

        SequencedCollection<String> seq = new ArrayList<>(List.of("a", "b", "c"));
        System.out.println("  sequenced: first=" + seq.getFirst()
                + " last=" + seq.getLast() + " reversed=" + seq.reversed());

        long t0 = System.nanoTime();
        try (ExecutorService vt = Executors.newVirtualThreadPerTaskExecutor()) {
            for (int i = 0; i < 10_000; i++) {
                vt.submit(() -> { Thread.sleep(100); return null; });
            }
        } // close() waits for every task
        System.out.println("  10,000 virtual threads x 100ms sleep finished in "
                + (System.nanoTime() - t0) / 1_000_000 + " ms  <-- try this with platform threads");
    }

    // ---------- After 21 ----------
    static void beyond21() {
        banner("Java 22-25 — not run here (needs a newer JDK)");
        System.out.println("""
                  22  unnamed variables:      for (var _ : list) count++;
                  22  FFM API final:          replaces JNI and sun.misc.Unsafe
                  24  stream gatherers:       stream.gather(Gatherers.windowFixed(3))
                  24  JEP 491:                synchronized no longer pins virtual threads
                  25  scoped values:          ScopedValue.where(CTX, v).run(...)  (ThreadLocal successor)
                  25  flexible constructors:  validate args before super(...)
                  --  withdrawn:              string templates, previewed twice then removed
                  --  still pending:          Valhalla value types, 10+ years in development""");
    }

    static void banner(String title) {
        System.out.println("\n== " + title + " " + "=".repeat(Math.max(0, 76 - title.length())));
    }
}
