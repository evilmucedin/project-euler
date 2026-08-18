import java.io.File;
import java.io.IOException;
import java.util.Locale;
import java.util.TreeMap;

import org.apache.avro.Schema;
import org.apache.avro.file.DataFileReader;
import org.apache.avro.file.DataFileWriter;
import org.apache.avro.generic.GenericData;
import org.apache.avro.generic.GenericDatumReader;
import org.apache.avro.generic.GenericDatumWriter;
import org.apache.avro.generic.GenericRecord;

/**
 * A small teaching tour of Apache Avro container files.
 *
 * Avro concepts demonstrated here:
 *
 *  1. Schema. Every Avro file is described by a JSON schema. The schema is
 *     written into the file header, so any reader can decode the file without
 *     out-of-band knowledge ("the data carries its own schema").
 *
 *  2. Object container file. The .avro on-disk format is a sequence of blocks
 *     of binary-encoded records, plus sync markers that make the file
 *     splittable (this is why Avro is popular in Hadoop/Spark pipelines).
 *
 *  3. GenericRecord. We use Avro's "generic" API: records are built and read
 *     dynamically against the schema, with no code generation step. (The
 *     alternative "specific" API generates Java classes from .avsc files.)
 *
 *  4. Appending. DataFileWriter.appendTo() reopens an existing container file
 *     and adds records to the end; the schema is read back from the header.
 *
 * Commands:
 *   create <file>                     create <file> with a few seed people
 *   add    <file> <name> <age> <city> append one person to <file>
 *   dump   <file>                     print every record in <file>
 *   ask    <file> <question>          answer a question from the data:
 *            count        how many people are in the file
 *            average-age  mean age over all people
 *            oldest       name and age of the oldest person
 *            by-city      number of people per city
 */
public final class AvroTour {
    /**
     * The schema, as JSON. A record named Person with three typed fields.
     * Parsed once into an org.apache.avro.Schema object below.
     */
    private static final String SCHEMA_JSON = """
            {
              "type": "record",
              "name": "Person",
              "namespace": "example.avro",
              "doc": "One row of our toy dataset",
              "fields": [
                {"name": "name", "type": "string"},
                {"name": "age",  "type": "int"},
                {"name": "city", "type": "string"}
              ]
            }
            """;

    private static final Schema SCHEMA = new Schema.Parser().parse(SCHEMA_JSON);

    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            usage();
            return;
        }
        String command = args[0];
        File file = new File(args[1]);
        switch (command) {
            case "create" -> create(file);
            case "add" -> {
                if (args.length != 5) {
                    usage();
                    return;
                }
                add(file, args[2], Integer.parseInt(args[3]), args[4]);
            }
            case "dump" -> dump(file);
            case "ask" -> {
                if (args.length != 3) {
                    usage();
                    return;
                }
                ask(file, args[2]);
            }
            default -> usage();
        }
    }

    /** Builds one Person record dynamically against the schema. */
    private static GenericRecord person(String name, int age, String city) {
        GenericRecord record = new GenericData.Record(SCHEMA);
        record.put("name", name);
        record.put("age", age);
        record.put("city", city);
        return record;
    }

    /**
     * Creates a brand-new Avro container file.
     *
     * DataFileWriter.create() writes the header (magic bytes, the schema as
     * JSON, codec, sync marker) and then each append() adds one record,
     * binary-encoded by GenericDatumWriter.
     */
    private static void create(File file) throws IOException {
        if (file.exists()) {
            System.err.println("refusing to overwrite existing file: " + file);
            System.exit(1);
        }
        GenericDatumWriter<GenericRecord> datumWriter = new GenericDatumWriter<>(SCHEMA);
        try (DataFileWriter<GenericRecord> writer = new DataFileWriter<>(datumWriter)) {
            writer.create(SCHEMA, file);
            writer.append(person("Ada", 36, "London"));
            writer.append(person("Grace", 45, "Arlington"));
            writer.append(person("Alan", 41, "Manchester"));
        }
        System.out.println("created " + file + " with 3 seed records");
        System.out.println("schema stored in the file header:\n" + SCHEMA.toString(true));
    }

    /**
     * Appends one record to an existing file.
     *
     * appendTo() reads the schema back out of the file header and positions
     * the writer at the end of the file, so new records are added in place —
     * no rewrite of existing data.
     */
    private static void add(File file, String name, int age, String city) throws IOException {
        GenericDatumWriter<GenericRecord> datumWriter = new GenericDatumWriter<>(SCHEMA);
        try (DataFileWriter<GenericRecord> writer = new DataFileWriter<>(datumWriter).appendTo(file)) {
            writer.append(person(name, age, city));
        }
        System.out.println("appended " + name + " (" + age + ", " + city + ") to " + file);
    }

    /**
     * Reads every record back.
     *
     * Note that GenericDatumReader needs no schema argument here: the reader
     * takes the writer schema from the file header. DataFileReader is an
     * Iterator over decoded records.
     */
    private static void dump(File file) throws IOException {
        GenericDatumReader<GenericRecord> datumReader = new GenericDatumReader<>();
        try (DataFileReader<GenericRecord> reader = new DataFileReader<>(file, datumReader)) {
            long i = 0;
            for (GenericRecord record : reader) {
                System.out.println(i++ + ": " + record);
            }
        }
    }

    /** Streams over the file once and answers a simple aggregate question. */
    private static void ask(File file, String question) throws IOException {
        long count = 0;
        long ageSum = 0;
        String oldestName = null;
        int oldestAge = Integer.MIN_VALUE;
        TreeMap<String, Long> byCity = new TreeMap<>();

        GenericDatumReader<GenericRecord> datumReader = new GenericDatumReader<>();
        try (DataFileReader<GenericRecord> reader = new DataFileReader<>(file, datumReader)) {
            for (GenericRecord record : reader) {
                // Avro decodes string fields as CharSequence (org.apache.avro.util.Utf8).
                String name = record.get("name").toString();
                int age = (Integer) record.get("age");
                String city = record.get("city").toString();

                count++;
                ageSum += age;
                if (age > oldestAge) {
                    oldestAge = age;
                    oldestName = name;
                }
                byCity.merge(city, 1L, Long::sum);
            }
        }

        switch (question) {
            case "count" -> System.out.println("count: " + count);
            case "average-age" -> {
                if (count == 0) {
                    System.out.println("average-age: no records");
                } else {
                    System.out.printf(Locale.ROOT, "average-age: %.2f%n", (double) ageSum / count);
                }
            }
            case "oldest" -> {
                if (oldestName == null) {
                    System.out.println("oldest: no records");
                } else {
                    System.out.println("oldest: " + oldestName + " (" + oldestAge + ")");
                }
            }
            case "by-city" -> byCity.forEach((city, n) -> System.out.println(city + ": " + n));
            default -> {
                System.err.println("unknown question: " + question);
                usage();
            }
        }
    }

    private static void usage() {
        System.err.println("""
                usage:
                  AvroTour create <file>
                  AvroTour add    <file> <name> <age> <city>
                  AvroTour dump   <file>
                  AvroTour ask    <file> count|average-age|oldest|by-city""");
        System.exit(2);
    }

    private AvroTour() {}
}
