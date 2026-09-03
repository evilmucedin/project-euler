# Geocoding quality comparison: Google Maps vs TomTom vs Mapbox

A small local harness that measures forward-geocoding quality of the three big
commercial geocoders across countries, against a hand-curated ground-truth
dataset.

## Idea

Geocoder quality is famously uneven across regions: a provider that is
excellent on US street addresses can be mediocre on Japanese block addressing
or Brazilian informal addresses. Marketing pages don't quantify this, so the
only way to pick a provider for a given market is to measure it yourself.

The methodology:

1. `addresses.json` holds ~30 test addresses in 8 countries (US, GB, DE, FR,
   JP, BR, IN, AU). Each has a ground-truth coordinate of a well-known
   building, so the correct answer is unambiguous and independently
   verifiable.
2. Each provider geocodes each address; we take the top result.
3. Error = great-circle (haversine) distance between the returned point and
   the ground truth.
4. Metrics per provider and per country:
   - **found%** — did the API return anything at all;
   - **hit@250m** — share of queries landing within 250 m of truth (the
     headline "did it find the right building/block" metric, robust to
     rooftop-vs-entrance differences);
   - **median / p90 / max error** in meters;
   - **average request latency**.

Responses are cached in `.geocode_cache.json`, so reruns are free and you can
add providers/addresses incrementally without re-spending quota.

## Running it

Python 3.8+, standard library only.

```sh
export GOOGLE_MAPS_API_KEY=...     # Google Geocoding API enabled
export TOMTOM_API_KEY=...          # TomTom Search API
export MAPBOX_ACCESS_TOKEN=...     # Mapbox Geocoding v6

python3 compare_geocoders.py
```

Providers whose key is missing are skipped with a note. A keyless `osm`
provider (OSM Nominatim, throttled to 1 req/s per its usage policy) is
included so the pipeline can be validated end-to-end without any credentials:

```sh
python3 compare_geocoders.py --providers osm
```

Useful flags: `--providers google,tomtom,mapbox`, `--countries US,JP,BR`,
`--no-cache`, `--csv out.csv`.

Output: markdown tables (per provider by country, and overall by provider) on
stdout, per-query details in `results.csv`.

## Getting keys (all have free tiers that cover this dataset ~1000x over)

- Google: https://console.cloud.google.com — enable "Geocoding API", create an
  API key. $200/month free credit.
- TomTom: https://developer.tomtom.com — free tier 2500 requests/day.
- Mapbox: https://account.mapbox.com — free tier 100k geocodes/month.

## Interpreting results

- `found%` low → coverage gaps in that country.
- `found%` high but `hit@250m` low → the provider "confidently" returns city
  or street centroids instead of the building (worst failure mode for
  delivery/logistics use cases).
- Compare countries within one provider to find its weak markets; compare
  providers within one country to pick a vendor for that market.
- POI-style queries (e.g. "Buckingham Palace") also exercise the landmark
  database, not just address parsing; the CSV lets you split those out.

## Sample run (keyless `osm` provider, 2026-09-03)

```
| country | n | found% | hit@250m% | median m | p90 m   | max m |
|---------|---|--------|-----------|----------|---------|-------|
| AU      | 2 | 100    | 50        | 6403     | 12750   | 12750 |
| BR      | 3 | 100    | 67        | 99       | 6363    | 6363  |
| DE      | 4 | 100    | 100       | 28       | 67      | 67    |
| FR      | 4 | 100    | 100       | 21       | 69      | 69    |
| GB      | 4 | 100    | 75        | 76       | 280     | 280   |
| IN      | 3 | 33     | 33        | 15       | 15      | 15    |
| JP      | 4 | 0      | 0         | -        | -       | -     |
| US      | 5 | 100    | 80        | 8        | 294     | 294   |
```

Already illustrative: Nominatim is near-perfect in Germany/France, misses
every romanized Japanese block address ("1 Chome-9-1 Marunouchi…"), finds only
1 of 3 Indian addresses, and puts "Bennelong Point, Sydney" 12.7 km away.
Exactly this kind of spread is what you want to see quantified per provider
before choosing one for a market.

## Caveats

- ~30 addresses is a smoke test, not a benchmark; conclusions about a
  provider/country pair should be confirmed by extending `addresses.json`
  (the format is trivial) — 50+ addresses per country of the *kind you care
  about* (residential, rural, new construction) is where it gets decisive.
- Ground truth is the building centroid; providers legitimately differ by
  10–100 m (rooftop vs entrance vs parcel), which is why hit@250m is the
  headline metric rather than median error.
- Top-1 result only; no evaluation of address-component correctness, only of
  the coordinate.
