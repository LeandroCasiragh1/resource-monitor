#include "../include/monitors.h"
#include "../include/utils.h"
#include <unistd.h>

int read_network_stats(const char *interface, NetworkStats *stats) {
    FILE *fp = fopen("/proc/net/dev", "r");
    if (!fp) {
        log_error("Failed to open /proc/net/dev");
        return -1;
    }

    char line[512];
    int found = 0;

    // Skip header lines
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char iface[32];
        unsigned long long rx_bytes, rx_packets, rx_errs, rx_drop;
        unsigned long long rx_fifo, rx_frame, rx_compressed, rx_multicast;
        unsigned long long tx_bytes, tx_packets, tx_errs, tx_drop;
        unsigned long long tx_fifo, tx_colls, tx_carrier, tx_compressed;

        int parsed = sscanf(line, "%[^:]: %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                          iface, &rx_bytes, &rx_packets, &rx_errs, &rx_drop,
                          &rx_fifo, &rx_frame, &rx_compressed, &rx_multicast,
                          &tx_bytes, &tx_packets, &tx_errs, &tx_drop,
                          &tx_fifo, &tx_colls, &tx_carrier, &tx_compressed);

        // Trim whitespace from interface name
        char *trimmed = iface;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;

        if (parsed >= 16 && strcmp(trimmed, interface) == 0) {
            strncpy(stats->interface, trimmed, sizeof(stats->interface) - 1);
            stats->rx_bytes = rx_bytes;
            stats->rx_packets = rx_packets;
            stats->rx_errors = rx_errs;
            stats->rx_dropped = rx_drop;
            stats->tx_bytes = tx_bytes;
            stats->tx_packets = tx_packets;
            stats->tx_errors = tx_errs;
            stats->tx_dropped = tx_drop;
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found ? 0 : -1;
}

int monitor_network(const char *interface, int duration_seconds, const char *output_file) {
    FILE *fp = safe_fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "timestamp,rx_bytes,rx_packets,rx_errors,tx_bytes,tx_packets,tx_errors,rx_rate_kb_s,tx_rate_kb_s\n");

    NetworkStats prev_stats, curr_stats;
    if (read_network_stats(interface, &prev_stats) != 0) {
        log_error("Failed to read initial network stats for interface %s", interface);
        fclose(fp);
        return -1;
    }

    for (int i = 0; i < duration_seconds; i++) {
        sleep(1);

        if (read_network_stats(interface, &curr_stats) != 0) {
            log_error("Failed to read network stats at iteration %d", i);
            continue;
        }

        // Calculate rates
        double rx_rate_kb = (curr_stats.rx_bytes - prev_stats.rx_bytes) / 1024.0;
        double tx_rate_kb = (curr_stats.tx_bytes - prev_stats.tx_bytes) / 1024.0;
        
        char timestamp[64];
        get_timestamp(timestamp, sizeof(timestamp));

        fprintf(fp, "%s,%llu,%llu,%llu,%llu,%llu,%llu,%.2f,%.2f\n",
                timestamp,
                curr_stats.rx_bytes, curr_stats.rx_packets, curr_stats.rx_errors,
                curr_stats.tx_bytes, curr_stats.tx_packets, curr_stats.tx_errors,
                rx_rate_kb, tx_rate_kb);
        
        fflush(fp);

        log_info("Network [%s] - RX: %.2f KB/s, TX: %.2f KB/s", 
                 interface, rx_rate_kb, tx_rate_kb);

        prev_stats = curr_stats;
    }

    fclose(fp);
    log_info("Network monitoring completed. Data saved to %s", output_file);
    return 0;
}
