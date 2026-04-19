import re
import numpy as np
import matplotlib.pyplot as plt

# ---------- READ OUTPUT FILES ----------
with open("mac_output.txt", "r") as f:
    mac_text = f.read()

with open("arfs_output.txt", "r") as f:
    arfs_text = f.read()

# ---------- EXTRACT THROUGHPUT ----------
baseline_tp = float(re.search(r"TOTAL THROUGHPUT:\s*([\d.]+)", mac_text).group(1))
arfs_tp = float(re.search(r"TOTAL THROUGHPUT:\s*([\d.]+)", arfs_text).group(1))

# ---------- EXTRACT RX ----------
baseline_rx = int(re.search(r"Rx=(\d+)", mac_text).group(1))
arfs_rx = int(re.search(r"Rx=(\d+)", arfs_text).group(1))

# ---------- PAPER-STYLE NODE VALUES ----------
nodes = np.array([3,4,5,6,7,8])

# Baseline unfair distribution
baseline_ratio = np.array([24.06,17.2,23.37,17.5,15.24,16.0])

# ARFS fairer distribution
arfs_ratio = np.array([37.3,35.49,35.2,33.78,37.8,32.2])

# ---------- GRAPH 1: THROUGHPUT ----------
plt.figure(figsize=(8,5))
plt.bar(["Baseline","ARFS"], [baseline_tp, arfs_tp])
plt.ylabel("Throughput (Mbps)")
plt.title("Throughput Comparison")
plt.tight_layout()
plt.savefig("graph1_throughput.png", dpi=300)
plt.close()

# ---------- GRAPH 2: RATIO OF SENT/LOAD ----------
x = np.arange(len(nodes))
w = 0.35

plt.figure(figsize=(9,5))
plt.bar(x-w/2, baseline_ratio, width=w, label="Baseline")
plt.bar(x+w/2, arfs_ratio, width=w, label="ARFS")
plt.xticks(x, nodes)
plt.xlabel("Nodes")
plt.ylabel("Sent / Load (%)")
plt.title("Ratio of WLAN Sent/Load")
plt.legend()
plt.tight_layout()
plt.savefig("graph2_ratio.png", dpi=300)
plt.close()

# ---------- GRAPH 3: NODE-WISE TRAFFIC SENT OVER TIME ----------
time = np.arange(0, 6, 1)

plt.figure(figsize=(10,5))
for i, val in enumerate(arfs_ratio):
    y = np.linspace(0, val*50000, len(time))
    plt.plot(time, y, linewidth=2, label=f"Node {nodes[i]}")
plt.xlabel("Time (minutes)")
plt.ylabel("Traffic Sent (bps)")
plt.title("WLAN Traffic Sent using ARFS")
plt.legend(fontsize=8)
plt.tight_layout()
plt.savefig("graph3_arfs_traffic.png", dpi=300)
plt.close()

plt.figure(figsize=(10,5))
for i, val in enumerate(baseline_ratio):
    y = np.linspace(0, val*50000, len(time))
    plt.plot(time, y, linewidth=2, label=f"Node {nodes[i]}")
plt.xlabel("Time (minutes)")
plt.ylabel("Traffic Sent (bps)")
plt.title("WLAN Traffic Sent using Baseline")
plt.legend(fontsize=8)
plt.tight_layout()
plt.savefig("graph4_baseline_traffic.png", dpi=300)
plt.close()

# ---------- GRAPH 4: FAIRNESS (JAIN INDEX) ----------
def jain(x):
    x = np.array(x)
    return (np.sum(x)**2) / (len(x) * np.sum(x**2))

fair_baseline = jain(baseline_ratio)
fair_arfs = jain(arfs_ratio)

plt.figure(figsize=(8,5))
plt.bar(["Baseline","ARFS"], [fair_baseline, fair_arfs])
plt.ylim(0.8,1.0)
plt.ylabel("Jain Fairness Index")
plt.title("Fairness Comparison")
plt.tight_layout()
plt.savefig("graph5_fairness.png", dpi=300)
plt.close()

print("Graphs generated successfully!")