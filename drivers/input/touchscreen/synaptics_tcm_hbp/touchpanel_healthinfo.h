/* Minimal no-op stub of the OPPO touchpanel_healthinfo facility.
 * The mainline Synaptics TCM-HBP port does not carry the health-monitor
 * telemetry framework; report calls compile to nothing. */
#ifndef _SYNA_HBP_HEALTHINFO_STUB_H_
#define _SYNA_HBP_HEALTHINFO_STUB_H_

#define HEALTH_REPORT "health_report"
#define tp_healthinfo_report(...) do { } while (0)

#endif
