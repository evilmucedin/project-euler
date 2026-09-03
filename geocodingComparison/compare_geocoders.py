#!/usr/bin/env python3
"""Compare forward-geocoding quality of Google Maps, TomTom and Mapbox by country.

For every test address (with a hand-curated ground-truth coordinate) the script
queries each provider, measures the great-circle error in meters, and reports
per-country and overall quality metrics:

  - found%    : share of queries that returned any result
  - hit@250m  : share of queries whose top result landed within 250 m of truth
  - median/p90/max error in meters (over found results)
  - mean request latency

Providers and credentials (environment variables):

  google    GOOGLE_MAPS_API_KEY
  tomtom    TOMTOM_API_KEY
  mapbox    MAPBOX_ACCESS_TOKEN
  osm       (none; Nominatim public endpoint, throttled to 1 req/s)

The keyless `osm` provider exists so the whole pipeline can be exercised
without any paid credentials. Responses are cached in .geocode_cache.json so
reruns do not spend API quota.

Usage:
  python3 compare_geocoders.py                     # all providers with credentials, plus osm
  python3 compare_geocoders.py --providers google,tomtom,mapbox
  python3 compare_geocoders.py --countries US,DE --no-cache
"""

import argparse
import json
import math
import os
import statistics
import sys
import time
import urllib.error
import urllib.parse
import urllib.request

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CACHE_PATH = os.path.join(SCRIPT_DIR, ".geocode_cache.json")
ADDRESSES_PATH = os.path.join(SCRIPT_DIR, "addresses.json")
HIT_THRESHOLD_M = 250.0
USER_AGENT = "geocoding-comparison-experiment/1.0 (github.com/evilmucedin/project-euler)"


def haversine_m(lat1, lon1, lat2, lon2):
    r = 6371000.0
    p1, p2 = math.radians(lat1), math.radians(lat2)
    dp = math.radians(lat2 - lat1)
    dl = math.radians(lon2 - lon1)
    a = math.sin(dp / 2) ** 2 + math.cos(p1) * math.cos(p2) * math.sin(dl / 2) ** 2
    return 2 * r * math.asin(math.sqrt(a))


def http_get_json(url):
    req = urllib.request.Request(url, headers={"User-Agent": USER_AGENT})
    with urllib.request.urlopen(req, timeout=30) as resp:
        return json.loads(resp.read().decode("utf-8"))


# --- Providers: each takes a query string, returns (lat, lon) or None -------


def geocode_google(query):
    key = os.environ["GOOGLE_MAPS_API_KEY"]
    url = "https://maps.googleapis.com/maps/api/geocode/json?" + urllib.parse.urlencode(
        {"address": query, "key": key}
    )
    data = http_get_json(url)
    if data.get("status") != "OK" or not data.get("results"):
        return None
    loc = data["results"][0]["geometry"]["location"]
    return loc["lat"], loc["lng"]


def geocode_tomtom(query):
    key = os.environ["TOMTOM_API_KEY"]
    url = (
        "https://api.tomtom.com/search/2/geocode/"
        + urllib.parse.quote(query, safe="")
        + ".json?"
        + urllib.parse.urlencode({"key": key, "limit": 1})
    )
    data = http_get_json(url)
    results = data.get("results") or []
    if not results:
        return None
    pos = results[0]["position"]
    return pos["lat"], pos["lon"]


def geocode_mapbox(query):
    token = os.environ["MAPBOX_ACCESS_TOKEN"]
    url = "https://api.mapbox.com/search/geocode/v6/forward?" + urllib.parse.urlencode(
        {"q": query, "access_token": token, "limit": 1}
    )
    data = http_get_json(url)
    features = data.get("features") or []
    if not features:
        return None
    lon, lat = features[0]["geometry"]["coordinates"]
    return lat, lon


def geocode_osm(query):
    url = "https://nominatim.openstreetmap.org/search?" + urllib.parse.urlencode(
        {"q": query, "format": "jsonv2", "limit": 1}
    )
    data = http_get_json(url)
    if not data:
        return None
    return float(data[0]["lat"]), float(data[0]["lon"])


PROVIDERS = {
    "google": (geocode_google, "GOOGLE_MAPS_API_KEY", 0.05),
    "tomtom": (geocode_tomtom, "TOMTOM_API_KEY", 0.25),
    "mapbox": (geocode_mapbox, "MAPBOX_ACCESS_TOKEN", 0.15),
    "osm": (geocode_osm, None, 1.1),  # Nominatim usage policy: max 1 req/s
}


# --- Runner ------------------------------------------------------------------


def load_cache(use_cache):
    if use_cache and os.path.exists(CACHE_PATH):
        with open(CACHE_PATH) as f:
            return json.load(f)
    return {}


def run_provider(name, dataset, cache, use_cache):
    fn, env_var, delay = PROVIDERS[name]
    rows = []
    for country, cases in dataset.items():
        for case in cases:
            cache_key = f"{name}::{case['query']}"
            if use_cache and cache_key in cache:
                entry = cache[cache_key]
            else:
                start = time.time()
                try:
                    coords = fn(case["query"])
                    error_msg = None
                except (urllib.error.URLError, KeyError, ValueError, OSError) as e:
                    coords, error_msg = None, str(e)
                latency = time.time() - start
                entry = {"coords": coords, "latency": latency, "error": error_msg}
                cache[cache_key] = entry
                time.sleep(delay)
            dist = None
            if entry["coords"]:
                dist = haversine_m(case["lat"], case["lon"], entry["coords"][0], entry["coords"][1])
            rows.append(
                {
                    "provider": name,
                    "country": country,
                    "query": case["query"],
                    "note": case["note"],
                    "found": entry["coords"] is not None,
                    "error_m": dist,
                    "latency_s": entry["latency"],
                    "request_error": entry.get("error"),
                }
            )
            status = f"{dist:8.0f} m" if dist is not None else "  MISS   "
            print(f"  [{name:6s}] {country} {status}  {case['note']}", file=sys.stderr)
    return rows


def summarize(rows, group_key):
    groups = {}
    for row in rows:
        groups.setdefault(row[group_key], []).append(row)
    out = []
    for key in sorted(groups):
        rs = groups[key]
        errors = [r["error_m"] for r in rs if r["error_m"] is not None]
        found = sum(1 for r in rs if r["found"])
        out.append(
            {
                "group": key,
                "n": len(rs),
                "found_pct": 100.0 * found / len(rs),
                "hit_pct": 100.0 * sum(1 for e in errors if e <= HIT_THRESHOLD_M) / len(rs),
                "median_m": statistics.median(errors) if errors else None,
                "p90_m": (sorted(errors)[max(0, math.ceil(0.9 * len(errors)) - 1)] if errors else None),
                "max_m": max(errors) if errors else None,
                "avg_latency_s": statistics.fmean(r["latency_s"] for r in rs),
            }
        )
    return out


def fmt(v, spec="{:.0f}"):
    return spec.format(v) if v is not None else "-"


def print_table(title, summaries):
    print(f"\n### {title}\n")
    print("| group | n | found% | hit@250m% | median m | p90 m | max m | avg lat s |")
    print("|---|---|---|---|---|---|---|---|")
    for s in summaries:
        print(
            f"| {s['group']} | {s['n']} | {s['found_pct']:.0f} | {s['hit_pct']:.0f} "
            f"| {fmt(s['median_m'])} | {fmt(s['p90_m'])} | {fmt(s['max_m'])} "
            f"| {s['avg_latency_s']:.2f} |"
        )


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--providers", help="comma-separated subset of: " + ",".join(PROVIDERS))
    parser.add_argument("--countries", help="comma-separated country codes to test (default: all)")
    parser.add_argument("--no-cache", action="store_true", help="ignore and overwrite the response cache")
    parser.add_argument("--csv", default=os.path.join(SCRIPT_DIR, "results.csv"), help="per-query CSV output path")
    args = parser.parse_args()

    with open(ADDRESSES_PATH) as f:
        dataset = json.load(f)
    if args.countries:
        wanted = {c.strip().upper() for c in args.countries.split(",")}
        dataset = {k: v for k, v in dataset.items() if k in wanted}

    if args.providers:
        names = [p.strip() for p in args.providers.split(",")]
        unknown = [p for p in names if p not in PROVIDERS]
        if unknown:
            sys.exit(f"unknown providers: {unknown}; available: {list(PROVIDERS)}")
    else:
        names = [p for p, (_, env, _) in PROVIDERS.items() if env is None or os.environ.get(env)]

    runnable = []
    for name in names:
        env_var = PROVIDERS[name][1]
        if env_var and not os.environ.get(env_var):
            print(f"skipping {name}: set {env_var} to enable it", file=sys.stderr)
        else:
            runnable.append(name)
    if not runnable:
        sys.exit("no runnable providers (no API keys set)")

    use_cache = not args.no_cache
    cache = load_cache(use_cache)
    all_rows = []
    for name in runnable:
        print(f"\nrunning provider: {name}", file=sys.stderr)
        all_rows.extend(run_provider(name, dataset, cache, use_cache))
        with open(CACHE_PATH, "w") as f:
            json.dump(cache, f, indent=1)

    with open(args.csv, "w") as f:
        cols = ["provider", "country", "query", "note", "found", "error_m", "latency_s", "request_error"]
        f.write(",".join(cols) + "\n")
        for r in all_rows:
            f.write(",".join('"' + str(r[c]).replace('"', '""') + '"' for c in cols) + "\n")

    print(f"\n# Geocoding quality comparison ({sum(len(v) for v in dataset.values())} addresses, "
          f"{len(dataset)} countries; hit threshold {HIT_THRESHOLD_M:.0f} m)")
    for name in runnable:
        rows = [r for r in all_rows if r["provider"] == name]
        print_table(f"{name} — by country", summarize(rows, "country"))
    print_table("Overall — by provider", summarize(all_rows, "provider"))
    print(f"\nPer-query details written to {args.csv}")


if __name__ == "__main__":
    main()
