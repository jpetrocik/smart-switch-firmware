#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# config.sh — Configure a smart-switch device over HTTP
#
# Sends an HTTP PUT to the device's /config endpoint with user-provided
# values as query parameters. Blank inputs are omitted from the request.
# ---------------------------------------------------------------------------

CURL_TIMEOUT=5
DEVICE_IP=""

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

die() { printf "Error: %s\n" "$1" >&2; exit 1; }

get_device_ip() {
    local ip
    ip=$(hostname -I 2>/dev/null | awk '{print $1}') || true
    if [[ -z "$ip" ]]; then
        die "Could not determine local IP address. Is the network up?"
    fi
    if ! printf '%s' "$ip" | grep -qE '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
        die "Detected IP '$ip' is not a valid IPv4 address."
    fi
    # Replace last octet with 1
    DEVICE_IP=$(echo "$ip" | sed -E 's/\.[0-9]+$/.1/')
}

check_device_reachable() {
    printf "Checking device at %s ... " "$DEVICE_IP"
    if curl -s --max-time "$CURL_TIMEOUT" "http://${DEVICE_IP}/" >/dev/null 2>&1; then
        printf "OK\n"
    else
        printf "FAILED\n"
        die "Cannot reach device at $DEVICE_IP. Is it powered on and connected?"
    fi
}

# ---------------------------------------------------------------------------
# Collect input
# ---------------------------------------------------------------------------

declare -A CFG

prompt_field() {
    local label="$1"
    local value=""
    read -rp "$label: " value
    CFG["$label"]="$value"
}

collect_config() {
    echo ""
    echo "=== Device Configuration ==="
    echo "(Leave blank to skip a field — it will not be sent)"
    echo ""

    prompt_field "device"
    prompt_field "room"
    prompt_field "location"
    prompt_field "mqttHost"
    prompt_field "ssid"
    prompt_field "password"

    echo ""
    local disable_led=""
    while true; do
        read -rp "Disable LED when on? (y/n): " disable_led
        case "$disable_led" in
            [Yy]) CFG["disableLed"]="true"; break ;;
            [Nn]) CFG["disableLed"]="false"; break ;;
            *)    printf "Please enter y or n.\n" ;;
        esac
    done
}

# ---------------------------------------------------------------------------
# Confirm & send
# ---------------------------------------------------------------------------

show_summary() {
    echo ""
    echo "=== Summary ==="
    printf "  %-14s %s\n" "device:"      "${CFG[device]:-<skip>}"
    printf "  %-14s %s\n" "room:"        "${CFG[room]:-<skip>}"
    printf "  %-14s %s\n" "location:"    "${CFG[location]:-<skip>}"
    printf "  %-14s %s\n" "mqttHost:"    "${CFG[mqttHost]:-<skip>}"
    printf "  %-14s %s\n" "ssid:"        "${CFG[ssid]:-<skip>}"
    printf "  %-14s %s\n" "password:"    "${CFG[password]:-<skip>}"
    printf "  %-14s %s\n" "disableLed:"  "${CFG[disableLed]}"
    echo ""
}

build_curl_args() {
    CURL_ARGS=()
    local fields=("device" "room" "location" "mqttHost" "ssid" "password" "disableLed")
    for f in "${fields[@]}"; do
        local val="${CFG[$f]:-}"
        if [[ -n "$val" ]]; then
            CURL_ARGS+=("${f}=${val}")
        fi
    done
    if [[ ${#CURL_ARGS[@]} -eq 0 ]]; then
        die "No values to send — all fields were blank."
    fi
}

send_config() {
    local url="http://${DEVICE_IP}/config"
    local first=true
    local query=""

    for arg in "${CURL_ARGS[@]}"; do
        if $first; then
            query="$arg"
            first=false
        else
            query="${query}&${arg}"
        fi
    done

    printf "Sending config to %s ... " "$url"
    local http_code
    http_code=$(curl -s -o /dev/null -w '%{http_code}' \
        --max-time "$CURL_TIMEOUT" \
        -X PUT "${url}?${query}")

    if [[ "$http_code" -ge 200 && "$http_code" -lt 300 ]]; then
        printf "OK (HTTP %s)\n" "$http_code"
        echo "Device will restart with new configuration."
    else
        printf "FAILED (HTTP %s)\n" "$http_code"
        die "Device rejected the request or is unavailable."
    fi
}

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

main() {
    get_device_ip
    check_device_reachable
    collect_config
    show_summary

    local confirm=""
    read -rp "Send this config? (y/n): " confirm
    case "$confirm" in
        [Yy]) ;;
        *)
            echo "Aborted."
            exit 0
            ;;
    esac

    build_curl_args
    send_config
}

main "$@"
