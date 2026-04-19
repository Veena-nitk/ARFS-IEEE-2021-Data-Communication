# 📡 Adaptive 802.11 MAC Protocol using ARFS (NS-3)

## 📌 Project Overview

This project implements and evaluates an adaptive MAC layer enhancement for IEEE 802.11 wireless networks based on the ARFS (Adaptive Rate and Fragmentation Scheme) algorithm.

The goal is to improve **throughput, fairness, and overall network efficiency** in multihop wireless networks by dynamically adjusting transmission parameters.

---

## 🎯 Objectives

* Implement baseline IEEE 802.11 MAC protocol in NS-3
* Implement ARFS-based adaptive MAC mechanism
* Compare both approaches under identical network conditions
* Analyze performance using multiple network metrics

---

## ⚙️ Methodology

The project is based on a 2021 IEEE research paper proposing ARFS.

We implemented:

* **Baseline Model:** Standard IEEE 802.11 MAC (CSMA/CA)
* **Proposed Model:** ARFS-based adaptive MAC

The ARFS algorithm dynamically adjusts:

* Transmission rate
* Fragmentation size
* Node priority (weight)

These adjustments are based on:

* Hop distance
* Collision probability
* Network conditions

---

## 🛠️ Tools Used

* **NS-3** – Network simulation
* **C++** – Implementation
* **FlowMonitor** – Performance analysis
* **AODV** – Routing protocol

---

## 📊 Performance Metrics

The system is evaluated using:

* Throughput
* End-to-End Delay
* Packet Loss
* Fairness Index
* WLAN Sent/Load Ratio
* Per-node Performance

---

## 📈 Results

The ARFS-based MAC protocol shows noticeable improvement over the standard IEEE 802.11 MAC:

* Higher throughput
* Reduced packet loss
* Improved fairness among nodes
* Better network utilization

---

## 📁 Repository Contents

* `arfs.cc` → ARFS-based MAC implementation
* `baseline.cc` → Standard IEEE 802.11 MAC simulation
* Graphs and results
* Report (LaTeX/PDF)

---

## 📚 Reference

Based on IEEE research paper:
*New Adaptive 802.11 MAC Protocol to Enhance Throughput and Fairness in Multihop Wireless Networks (2021)*

---

## Conclusion

This project demonstrates that adaptive MAC layer optimization significantly enhances performance in multihop wireless networks by reducing collisions and improving fairness through dynamic parameter tuning.

