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

Providers whose key is missing are skipped with a note. Three keyless
providers are included so a real comparison runs with zero credentials:
`osm` (OSM Nominatim, throttled to 1 req/s per its usage policy), `photon`
(komoot's OSM-based geocoder), and `arcgis` (Esri World Geocoder — keyless
access is permitted for non-stored geocoding):

```sh
python3 compare_geocoders.py --providers osm,photon,arcgis
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

## Sample run: the three free providers (2026-09-03)

Overall:

```
| provider | n  | found% | hit@250m% | median m | p90 m | max m | avg lat s |
|----------|----|--------|-----------|----------|-------|-------|-----------|
| arcgis   | 29 | 100    | 97        | 51       | 156   | 3338  | 0.20      |
| photon   | 29 | 97     | 86        | 30       | 359   | 8136  | 0.92      |
| osm      | 29 | 79     | 66        | 28       | 294   | 12750 | 0.62      |
```

hit@250m by country:

```
| country | osm | photon | arcgis |
|---------|-----|--------|--------|
| AU      | 50  | 100    | 100    |
| BR      | 67  | 67     | 67     |
| DE      | 100 | 100    | 100    |
| FR      | 100 | 100    | 100    |
| GB      | 75  | 100    | 100    |
| IN      | 33  | 67     | 100    |
| JP      | 0   | 75     | 100    |
| US      | 80  | 80     | 100    |
```

Takeaways from even this small run:

- **ArcGIS** (a commercial-grade geocoder) is the most robust: 100% found,
  only one >250 m miss (Christ the Redeemer, 3.3 km), and ~5x lower latency.
- **Nominatim vs Photon** is a like-for-like ablation — same underlying OSM
  data, different query parsing — and parsing alone lifts Japan from 0% to
  75% and India from 33% to 67%. Strict structured parsing is what fails on
  romanized Japanese block addresses, not missing map data.
- Precision vs robustness: where OSM-based results are correct they are often
  *more* precise than ArcGIS (median 28–30 m vs 51 m — OSM points at the
  building, ArcGIS at the parcel/street), but they fail harder when parsing
  goes wrong (8–12 km misses).
- Brazil's "Christ the Redeemer" address misses on all three — a reminder
  that some failure modes are shared, and per-country data quality is the
  bottleneck no parser can fix.

## Note on generating commercial keys non-interactively

- **Google**: if you have a *personal* GCP project with billing, a key can be
  minted from the CLI with no web console:
  `gcloud services enable geocoding-backend.googleapis.com api-keys.googleapis.com`
  then `gcloud services api-keys create --display-name=geocoding-comparison`.
  (Not done here automatically: the only configured gcloud account on this
  machine is a corporate one.)
- **TomTom / Mapbox**: signup requires interactive email verification, so
  keys must be created once by hand at the links above.

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
