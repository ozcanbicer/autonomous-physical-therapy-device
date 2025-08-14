# Autonomous Physical Therapy Device

[![CI/CD Pipeline](https://github.com/ozcanbicer/autonomous-physical-therapy-device/workflows/CI/CD%20Pipeline/badge.svg)](https://github.com/ozcanbicer/autonomous-physical-therapy-device/actions)
[![License](https://img.shields.io/badge/License-Proprietary-red.svg)](LICENSE)
[![IEC 62304](https://img.shields.io/badge/IEC%2062304-Class%20C-critical)](https://www.iso.org/standard/38421.html)
[![HIPAA](https://img.shields.io/badge/HIPAA-Compliant-green)](https://www.hhs.gov/hipaa/index.html)

## 🏥 Overview

The Autonomous Physical Therapy Device is an AI-powered medical device that provides personalized physical therapy without requiring a human therapist present. Using advanced computer vision and pose estimation, it guides patients through exercises while monitoring form and providing real-time feedback.

### 🎯 Key Features

- **AI-Powered Analysis**: Real-time pose estimation and movement analysis using MediaPipe
- **Hardware Flexibility**: Seamless migration from Intel RealSense D435 to ORBBEC Femto Mega
- **Medical Grade**: IEC 62304 Class C compliant for highest safety standards
- **HIPAA Compliant**: Secure patient data handling and storage
- **Real-time Processing**: <500ms end-to-end latency for immediate feedback
- **Cross-platform**: Supports development on x86 and production on ARM (Jetson Orin NX)

## 🏗️ Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Camera Layer  │───▶│ Hardware Abstract│───▶│  AI Processing  │
│  (D435/Femto)   │    │     Layer        │    │   (MediaPipe)   │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                │
                                ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│  Mobile Apps    │◀───│  Communication  │◀───│  Core System    │
│ (React Native)  │    │  (WebSocket/MQTT)│    │   Components    │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

## 🔧 Technical Specifications

### Hardware Requirements

**Development Environment:**
- Intel RealSense D435 Camera
- ASUS TUF Gaming A15 (or equivalent)
- 16GB+ RAM, NVIDIA GPU recommended

**Production Environment:**
- ORBBEC Femto Mega Camera
- NVIDIA Jetson Orin NX 16GB
- <25W power consumption
- Active cooling system

### Software Stack

- **Primary Language**: C++17 (IEC 62304 compliant)
- **AI Framework**: MediaPipe C++ with TensorRT optimization
- **Build System**: CMake with cross-compilation support
- **Testing**: GoogleTest with 95%+ coverage
- **Mobile**: React Native (TypeScript)
- **Database**: Supabase (HIPAA compliant)

## 🚀 Quick Start

### Prerequisites

```bash
# Ubuntu 22.04 LTS
sudo apt update && sudo apt install -y \
    build-essential cmake git \
    librealsense2-dev \
    libopencv-dev \
    python3-dev python3-pip

# Install Python dependencies
pip3 install numpy pytest
```

### Building

```bash
# Clone repository
git clone https://github.com/ozcanbicer/autonomous-physical-therapy-device.git
cd autonomous-physical-therapy-device

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Development Setup

```bash
# Setup development environment
./scripts/setup_dev_env.sh

# Run with D435 camera
./build/bin/therapy_device --camera=d435

# Run with Femto Mega camera
./build/bin/therapy_device --camera=femto_mega
```

## 📁 Project Structure

```
autonomous-physical-therapy-device/
├── .cursor/                    # Cursor AI configuration
├── src/
│   ├── cpp/                    # C++ source code
│   │   ├── core/              # Core system components
│   │   ├── hardware/          # Hardware abstraction
│   │   ├── ai/                # AI/ML processing
│   │   ├── communication/     # Communication layer
│   │   └── utils/             # Utility functions
│   ├── python/                # Python support code
│   │   ├── ml/                # ML model development
│   │   ├── analysis/          # Data analysis
│   │   └── tools/             # Development tools
│   └── mobile/                # TypeScript mobile apps
│       └── app/               # React Native
├── include/                   # C++ header files
├── third_party/               # External libraries
├── tests/                     # Test suite
│   ├── unit/                  # Unit tests
│   ├── integration/           # Integration tests
│   └── performance/           # Performance tests
├── docs/                      # Documentation
│   ├── requirements/          # Requirements
│   ├── design/                # Design documents
│   ├── api/                   # API documentation
│   ├── compliance/            # Compliance docs
│   └── sprint/                # Sprint documentation
├── config/                    # Configuration files
├── scripts/                   # Build and deployment scripts
├── cmake/                     # CMake modules
└── .github/                   # GitHub workflows
```

## 🧪 Testing

### Running Tests

```bash
# Unit tests
cd build && ctest -L unit

# Integration tests
ctest -L integration

# Performance tests
ctest -L performance

# All tests with coverage
make test_coverage
```

### Test Coverage

- **Target**: 95%+ coverage for critical components
- **Tools**: gcov, lcov for coverage analysis
- **CI Integration**: Automatic coverage reporting

## 📋 Compliance

### IEC 62304 Class C

- **Safety Classification**: Class C (highest risk)
- **Documentation**: Complete Design History File (DHF)
- **Traceability**: Requirements ↔ Code ↔ Tests
- **Risk Management**: ISO 14971 compliant

### Regulatory Approval

- **FDA**: 510(k) submission ready
- **CE Mark**: MDR compliance
- **Quality System**: ISO 13485

## 🔒 Security & Privacy

- **HIPAA Compliance**: Patient data encryption and access controls
- **GDPR Ready**: Data protection and privacy by design
- **Network Security**: TLS 1.3 for all communications
- **Audit Trails**: Complete logging and monitoring

## 🤝 Contributing

This is a medical device project with strict compliance requirements. All contributions must:

1. Follow IEC 62304 development practices
2. Include comprehensive tests and documentation
3. Pass static analysis and security scans
4. Be reviewed by qualified personnel

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

## 📄 License

Proprietary - All rights reserved. This medical device software is subject to regulatory approval and licensing requirements.

## 📞 Support

For technical support, compliance questions, or regulatory inquiries:

- **Technical Issues**: [GitHub Issues](https://github.com/ozcanbicer/autonomous-physical-therapy-device/issues)
- **Compliance**: compliance@company.com
- **Security**: security@company.com

---

**⚠️ Medical Device Notice**: This software is intended for use as a medical device and must be operated according to regulatory guidelines and local medical device regulations.
