#ifndef _COVERAGE_H
#define _COVERAGE_H

#include "bed.h"
#include "htslib/sam.h"
#include "report.h"
#include <stdbool.h>
#include <stdint.h>

#define BUFFER 100
#define MISS_BUFFER 500

enum target_state { TARGET_OUT = 0, TARGET_BUFFER, TARGET_IN };
typedef enum target_state target_state_t;

/* Coverage info structure */
struct coverage_info {
    /* Length of cov_histo (>= higest coverage value) */
    size_t cov_histo_len;
    uint64_t *cov_histo; /* Coverage histogram */
};
typedef struct coverage_info coverage_info_t;

coverage_info_t *coverage_info_init();
void coverage_info_destroy(coverage_info_t *ci);

/* Capture metrics structure */
struct capture_metrics {
    /* Read info */
    uint64_t r_total;         /* Total reads */
    uint64_t r_aligned;       /* Aligned reads */
    uint64_t r_paired;        /* Paired reads */
    uint64_t r_paired_w_mate; /* Paired reads with mate pair mapped */
    uint64_t r_dup;           /* Duplicate reads */
    uint64_t r_in_target;     /* Reads mapped in a target region */
    uint64_t r_in_buffer;     /* Reads mapped only in a target's buffer */
    uint64_t r_out_target;    /* Reads not mapped to a target or buffer */

    /* Base info */
    uint64_t b_total;          /* Total bases */
    uint64_t b_aligned;        /* Aligned bases */
    uint64_t b_targeted;       /* Bases in target regions */
    uint64_t b_buffer;         /* Bases in target buffers */
    uint64_t b_masked;         /* Masked bases */
    uint64_t b_1_plus_hits;    /* Target bases with >= 1X coverage */
    uint64_t b_10_plus_hits;   /* Target bases with >= 10X coverage */
    uint64_t b_20_plus_hits;   /* Target bases with >= 20X coverage */
    uint64_t b_30_plus_hits;   /* Target bases with >= 30X coverage */
    uint64_t b_40_plus_hits;   /* Target bases with >= 40X coverage */
    uint64_t b_50_plus_hits;   /* Target bases with >= 50X coverage */
    uint64_t b_100_plus_hits;  /* Target bases with >= 100X coverage */
    uint64_t b_500_plus_hits;  /* Target bases with >= 500X coverage */
    uint64_t b_1000_plus_hits; /* Target bases with >= 1000X coverage */

    /* Target info */
    uint64_t t_total;       /* Number of targets from target file */
    uint64_t t_hit;         /* Number of targets with >= 1X coverage */
    uint64_t t_buffers_hit; /* Number of target buffer regions hit */
                            /*
                             * Contiguous regions of coverage outside of target regions with >= 20X
                             * coverage at one or more base positions.
                             */
    uint64_t t_non_target_good_hits;

    /* Coverage info */
    uint64_t c_total;  /* Sum of target coverage values (for median, average) */
    uint64_t c_median; /* Median target coverage */
};
typedef struct capture_metrics capture_metrics_t;

capture_metrics_t *capture_metrics_init();
void capture_metrics_finalize(capture_metrics_t *cm, coverage_info_t *ci,
                              bed_t *ti);
void capture_metrics_destroy(capture_metrics_t *cm);

void incr_cov_histo(coverage_info_t *ci, uint32_t cov);
void handle_wgs_coverage(const uint32_t *coverage, capture_metrics_t *cm,
                         coverage_info_t *ci, int32_t chrom_len);
void handle_target_coverage(const uint32_t *coverage, capture_metrics_t *cm,
                            coverage_info_t *ci, bed_t *ti, int32_t chrom_idx,
                            const char *chrom, int32_t chrom_len);
void clear_coverage(uint32_t *coverage, int32_t start, int32_t end, int32_t chrom_len);
void handle_miss_reads(uint32_t *coverage, capture_metrics_t *cm, bed_t *ti,
                       int32_t chrom_idx, int32_t chrom_len);
void handle_coverage_mask(uint32_t *coverage, bed_t *cov_mask_ti,
                          int32_t chrom_idx, int32_t chrom_len);
void handle_coverage_mask_target(uint8_t *target_cov, capture_metrics_t *cm,
                                 bed_t *cov_mask_ti, int32_t chrom_idx,
                                 int32_t chrom_len);
void set_target_cov(uint8_t *target_cov, capture_metrics_t *cm, bed_t *ti,
                    int32_t chrom_idx, int32_t chrom_len);
void capture_process_record(bam1_t *rec, uint32_t *coverage,
                            const uint8_t *target_cov,
                            capture_metrics_t *cm_wgs,
                            capture_metrics_t *cm_cap, int32_t chrom_len,
                            bool remove_dups);
void capture_report(report_t *report, capture_metrics_t *cm, bed_t *ti);

#endif /* _COVERAGE_H */
