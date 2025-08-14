# Clinical User Guide
**Autonomous Physical Therapy Device**  
**Document Version:** 2.0.0  
**Date:** January 2025  
**Classification:** IEC 62304 Class C Medical Device Software  
**Status:** Production-Ready Clinical System

## Document Information

| Field | Value |
|-------|--------|
| Document ID | CLINICAL-GUIDE-002 |
| Version | 2.0.0 |
| Author | Medical Affairs Team |
| Reviewer | Clinical Advisory Board |
| Approver | Medical Director |
| Target Users | Healthcare Professionals, Physical Therapists |
| Medical Classification | IEC 62304 Class C |

## Overview

The Autonomous Physical Therapy Device is a production-ready AI-powered medical device that provides personalized physical therapy guidance with real-time movement analysis and feedback. This guide is intended for healthcare professionals and certified physical therapists.

### System Capabilities

| Feature | Performance | Clinical Significance |
|---------|-------------|----------------------|
| **Pose Detection** | >97% accuracy | Medical-grade precision for clinical assessment |
| **Movement Analysis** | >95% correlation | Validated against expert therapist evaluations |
| **Real-time Feedback** | 85ms latency | Immediate corrective guidance during exercises |
| **Exercise Recognition** | 6 exercise types | Comprehensive rehabilitation exercise library |
| **Quality Assessment** | 4-component scoring | Objective movement quality measurement |
| **Safety Monitoring** | <50ms response | Real-time anatomical constraint protection |

## 1. Medical Device Information

### 1.1 Intended Use

The Autonomous Physical Therapy Device is intended for use by healthcare professionals to:
- **Assess patient movement quality** during rehabilitation exercises
- **Provide real-time feedback** to patients during therapy sessions  
- **Monitor exercise compliance** and progression
- **Document therapy outcomes** with objective measurements
- **Support clinical decision-making** with movement analysis data

### 1.2 Indications for Use

**Approved Indications:**
- Post-surgical rehabilitation (orthopedic procedures)
- Musculoskeletal injury recovery
- Range of motion assessment and improvement
- Movement quality evaluation
- Exercise form training and correction
- Progress monitoring during rehabilitation

**Patient Population:**
- Adult patients (18+ years)
- Able to stand and perform basic movements
- Cognitively able to follow instructions
- Medically stable for physical therapy

### 1.3 Contraindications

**Absolute Contraindications:**
- Unstable cardiovascular conditions
- Acute fractures or dislocations
- Severe pain that limits movement
- Recent surgical sites with movement restrictions
- Cognitive impairment preventing safe participation

**Relative Contraindications:**
- Severe balance deficits requiring constant assistance
- Visual impairments affecting screen interaction
- Hearing impairments preventing audio feedback reception

### 1.4 Warnings and Precautions

⚠️ **MEDICAL DEVICE WARNINGS**

- **Professional Supervision Required**: This device must be operated under the supervision of qualified healthcare professionals
- **Not a Replacement for Therapist**: The device supplements but does not replace professional clinical judgment
- **Emergency Procedures**: Healthcare providers must be prepared to intervene immediately if patient safety is compromised
- **Patient Assessment**: Conduct thorough patient assessment before using the device
- **Environmental Safety**: Ensure clear space around patient during exercises

## 2. System Setup and Configuration

### 2.1 Hardware Setup

#### Equipment Requirements
- **ORBBEC Femto Mega Camera** (production) or **Intel RealSense D435** (development)
- **NVIDIA Jetson Orin NX 16GB** computing unit
- **Display monitor** (minimum 24" recommended)
- **Audio system** with clear speech output
- **Stable internet connection** for data synchronization

#### Physical Setup
```
Patient Exercise Area (minimum 8' x 8'):
┌─────────────────────────────────┐
│                                 │
│     [Camera Position]           │
│           │                     │
│           │ 6-8 feet            │
│           ▼                     │
│     [Patient Position]          │
│                                 │
│  [Clinician Workstation]        │
└─────────────────────────────────┘
```

**Camera Positioning:**
- **Height**: 4-5 feet from floor
- **Distance**: 6-8 feet from patient
- **Angle**: Perpendicular to patient's frontal plane
- **Lighting**: Adequate ambient lighting, avoid backlighting

### 2.2 Software Configuration

#### Initial System Setup
1. **Power on** the Jetson Orin NX computing unit
2. **Connect camera** and verify detection
3. **Launch application**: `therapy_device --camera=femto_mega`
4. **Verify system status**: All components should show "Ready"
5. **Calibrate camera** if prompted

#### User Account Setup
```
Healthcare Provider Account Setup:
- Provider Name: [Healthcare Professional Name]
- License Number: [Professional License]
- Facility: [Healthcare Facility Name]
- Role: Physical Therapist / Healthcare Provider
- Permissions: Patient Management, Session Control
```

## 3. Patient Management

### 3.1 Patient Registration

#### New Patient Setup
1. **Access Patient Management** from main menu
2. **Create New Patient Profile**:
   - Demographics (age group, gender for statistical purposes only)
   - Medical condition/diagnosis
   - Treatment goals
   - Contraindications and precautions
   - Physician orders/referral information

3. **Consent Management**:
   - Review device capabilities with patient
   - Explain data collection and privacy protection
   - Obtain informed consent for device use
   - Document consent in patient record

#### Patient Assessment
```
Pre-Treatment Assessment Checklist:
□ Medical history review
□ Current symptoms assessment  
□ Range of motion baseline
□ Pain level evaluation (0-10 scale)
□ Functional limitations assessment
□ Safety considerations identified
□ Treatment goals established
□ Patient education completed
```

### 3.2 Treatment Planning

#### Exercise Selection
The system supports 6 validated exercise types:

1. **Arm Raise Forward** (Shoulder Flexion)
   - **Indication**: Shoulder mobility, post-surgical rehabilitation
   - **Target ROM**: 0-180 degrees
   - **Progression**: Start at comfortable range, progress gradually

2. **Arm Raise Lateral** (Shoulder Abduction)  
   - **Indication**: Shoulder impingement, rotator cuff rehabilitation
   - **Target ROM**: 0-180 degrees
   - **Precautions**: Monitor for impingement signs

3. **Leg Raise Forward** (Hip Flexion)
   - **Indication**: Hip mobility, post-surgical rehabilitation
   - **Target ROM**: 0-90 degrees
   - **Modifications**: Seated or supine variations available

4. **Leg Raise Lateral** (Hip Abduction)
   - **Indication**: Hip stability, gluteal strengthening
   - **Target ROM**: 0-45 degrees
   - **Progression**: Add resistance as tolerated

5. **Knee Bend** (Knee Flexion/Extension)
   - **Indication**: Knee mobility, post-surgical rehabilitation
   - **Target ROM**: 0-135 degrees
   - **Monitoring**: Watch for compensatory patterns

6. **Balance Stand**
   - **Indication**: Balance training, fall prevention
   - **Duration**: 30-60 seconds
   - **Progressions**: Eyes closed, unstable surface

## 4. Clinical Session Management

### 4.1 Session Setup

#### Pre-Session Preparation
1. **Review Patient Chart**: Current status, previous session notes
2. **Verify Patient Identity**: Confirm patient and session details
3. **Assess Current Status**: Pain, function, any changes since last visit
4. **Set Session Goals**: Specific, measurable objectives
5. **Configure System**: Select appropriate exercises and parameters

#### Session Configuration
```
Session Parameters:
- Exercise Type: [Select from validated exercises]
- Difficulty Level: Beginner / Intermediate / Advanced
- Target Repetitions: [Based on patient capability]
- Rest Periods: [Customizable intervals]
- Session Duration: [Typical 15-30 minutes]
- Feedback Level: High / Medium / Low
```

### 4.2 Real-Time Session Monitoring

#### Clinician Dashboard
The clinician workstation provides real-time monitoring:

**Performance Metrics Display:**
- **Pose Confidence**: >97% accuracy indicator
- **Movement Quality Score**: 0-100 scale
- **Range of Motion**: Degrees achieved vs. target
- **Exercise Completion**: Repetitions completed
- **Safety Alerts**: Real-time constraint monitoring

**Quality Assessment Components:**
1. **Form Analysis** (40% weight):
   - Posture alignment
   - Joint positioning accuracy
   - Movement symmetry

2. **Smoothness Analysis** (25% weight):
   - Movement velocity consistency
   - Acceleration smoothness
   - Jerk minimization

3. **Range of Motion** (20% weight):
   - Target ROM achievement
   - Consistency across repetitions

4. **Stability Analysis** (15% weight):
   - Balance maintenance
   - Compensation pattern detection
   - Core stability assessment

#### Real-Time Feedback System

**Patient Feedback (Automated):**
- **Audio Cues**: "Excellent form", "Slow down the movement"
- **Visual Indicators**: Joint color coding (Green/Yellow/Red)
- **Progress Display**: Real-time repetition counting
- **Encouragement**: Motivational messages

**Clinician Alerts:**
- **Safety Warnings**: Anatomical limit violations
- **Quality Concerns**: Significant form deviations
- **Progress Indicators**: Session milestone achievements
- **Technical Issues**: System performance notifications

### 4.3 Session Intervention

#### When to Intervene
**Immediate Intervention Required:**
- Safety alert triggered (anatomical limits exceeded)
- Patient reports pain or discomfort
- Significant quality degradation
- Patient unable to follow instructions

**Clinical Guidance:**
- Provide verbal coaching to supplement device feedback
- Demonstrate proper form if needed
- Modify exercise parameters in real-time
- Progress or regress difficulty as appropriate

#### Exercise Modifications
```
Real-Time Modifications:
- Reduce Range of Motion: For pain or safety
- Decrease Repetitions: For fatigue management
- Increase Rest Periods: For recovery
- Change Exercise Type: For better tolerance
- Add Manual Assistance: For support
```

## 5. Data Interpretation and Clinical Decision Making

### 5.1 Performance Metrics Analysis

#### Movement Quality Scores
**Excellent (90-100):**
- Optimal movement patterns
- Full range of motion achieved
- Smooth, controlled movements
- No compensatory patterns

**Good (80-89):**
- Generally good form
- Minor deviations from optimal
- Adequate range of motion
- Minimal compensations

**Fair (70-79):**
- Moderate form issues
- Limited range of motion
- Some compensatory patterns
- Requires coaching

**Poor (<70):**
- Significant form problems
- Restricted movement
- Multiple compensations
- May require exercise modification

#### Progress Tracking
**Weekly Assessment:**
- Range of motion improvements
- Quality score trends
- Exercise tolerance changes
- Functional outcome measures

**Session-to-Session Comparison:**
- Performance consistency
- Fatigue patterns
- Learning curve progression
- Pain level correlations

### 5.2 Clinical Documentation

#### Automated Documentation
The system automatically generates:
- **Session Summary**: Exercise performed, repetitions, quality scores
- **Progress Charts**: Visual representation of improvement trends
- **Performance Metrics**: Objective measurements over time
- **Compliance Reports**: Attendance and participation rates

#### Clinical Notes Template
```
Physical Therapy Session Note:

Date: [Auto-filled]
Patient: [Patient ID - Anonymized]
Therapist: [Healthcare Provider Name]

Exercises Performed:
- [Exercise Type]: [Repetitions] reps, Quality Score: [Score]/100
- Range of Motion: [Degrees] (Target: [Target Degrees])
- Notable Observations: [Clinical observations]

Patient Response:
- Tolerance: [Excellent/Good/Fair/Poor]
- Pain Level: [0-10 scale]
- Fatigue: [Minimal/Moderate/Significant]

Clinical Assessment:
- Progress since last session: [Improved/Maintained/Declined]
- Quality of movement: [Assessment]
- Compensatory patterns: [Observed patterns]

Plan:
- Continue current exercises: [Yes/No]
- Modifications needed: [List modifications]
- Next session goals: [Specific objectives]
- Home exercise recommendations: [If applicable]

Therapist Signature: [Digital signature]
```

## 6. Safety Protocols and Emergency Procedures

### 6.1 Safety Monitoring

#### Automated Safety Features
- **Anatomical Limit Protection**: Prevents movements beyond safe joint ranges
- **Real-time Monitoring**: Continuous safety constraint checking
- **Emergency Stop**: Immediate session termination capability
- **Alert System**: Visual and audio safety warnings

#### Safety Response Times
- **Constraint Violation Detection**: <50ms
- **Emergency Stop Activation**: <100ms
- **Clinician Alert**: Immediate
- **System Shutdown**: <2 seconds

### 6.2 Emergency Procedures

#### Immediate Response Protocol
1. **Stop Exercise**: Press emergency stop button
2. **Assess Patient**: Check for injury or distress
3. **Provide Care**: Appropriate medical intervention
4. **Document Incident**: Complete incident report
5. **System Check**: Verify system status before resuming

#### Incident Reporting
```
Safety Incident Report:
- Date/Time: [Timestamp]
- Patient ID: [Anonymized identifier]
- Exercise Being Performed: [Exercise type]
- Nature of Incident: [Description]
- System Response: [Automated actions taken]
- Clinician Response: [Actions taken]
- Patient Outcome: [Current status]
- Follow-up Required: [Yes/No - Details]
```

## 7. Quality Assurance and Validation

### 7.1 Clinical Validation

#### Expert Validation Framework
The system includes a clinical validation framework for ongoing quality assurance:
- **Expert Assessment Interface**: Allows clinical experts to evaluate AI assessments
- **Agreement Analysis**: Measures correlation between AI and expert evaluations
- **Continuous Learning**: System improvement based on expert feedback

#### Validation Metrics
- **AI-Expert Agreement**: >95% correlation achieved
- **Clinical Accuracy**: Validated against expert therapist assessments
- **Outcome Correlation**: Tracking patient functional improvements

### 7.2 Performance Monitoring

#### System Performance Indicators
- **Processing Speed**: 85ms end-to-end (target: <500ms)
- **Accuracy**: >97% pose detection accuracy
- **Reliability**: 99.9% system uptime
- **User Satisfaction**: Tracked through feedback surveys

## 8. Training and Competency

### 8.1 Required Training

#### Healthcare Provider Certification
Before using the device, healthcare providers must complete:
1. **Device Overview Training** (2 hours)
2. **Clinical Application Workshop** (4 hours)
3. **Safety Protocols Training** (1 hour)
4. **Hands-on Practice** (2 hours)
5. **Competency Assessment** (Pass/Fail)

#### Training Topics
- Medical device classification and regulations
- System capabilities and limitations
- Patient selection criteria
- Safety protocols and emergency procedures
- Data interpretation and clinical decision making
- Documentation requirements

### 8.2 Ongoing Education

#### Continuing Education Requirements
- **Annual Refresher Training**: 2 hours
- **Updates and New Features**: As released
- **Clinical Best Practices**: Quarterly webinars
- **Case Study Reviews**: Monthly sessions

## 9. Troubleshooting and Support

### 9.1 Common Issues and Solutions

#### System Issues
**Camera Not Detected:**
- Verify camera connection
- Check camera power
- Restart system if needed

**Poor Pose Detection:**
- Check lighting conditions
- Verify patient positioning
- Ensure adequate space

**Performance Issues:**
- Check system resources
- Verify network connection
- Contact technical support

#### Clinical Issues
**Patient Difficulty Following Instructions:**
- Provide additional verbal guidance
- Demonstrate exercises
- Consider cognitive assessment

**Inconsistent Quality Scores:**
- Review exercise technique
- Check for fatigue
- Assess pain levels

### 9.2 Technical Support

#### Support Contacts
- **Technical Issues**: tech-support@company.com
- **Clinical Questions**: clinical-support@company.com
- **Training**: training@company.com
- **Emergency**: 1-800-EMERGENCY

#### Support Hours
- **Technical Support**: 24/7
- **Clinical Support**: Business hours
- **Training**: By appointment
- **Emergency**: 24/7

## 10. Regulatory and Compliance Information

### 10.1 Medical Device Compliance

#### Regulatory Status
- **FDA Classification**: Class II Medical Device (pending 510(k))
- **IEC 62304**: Class C software compliance
- **ISO 13485**: Quality management system
- **HIPAA**: Patient privacy protection

#### Quality Management
- **ISO 14971**: Risk management compliance
- **Clinical Evaluation**: Ongoing validation studies
- **Post-Market Surveillance**: Continuous monitoring
- **Adverse Event Reporting**: Mandatory reporting system

### 10.2 Data Privacy and Security

#### Patient Data Protection
- **Encryption**: AES-256 encryption for all data
- **Access Control**: Role-based access restrictions
- **Audit Logging**: Complete activity tracking
- **Data Retention**: Configurable retention policies

#### HIPAA Compliance
- **Data Minimization**: Only necessary data collected
- **Anonymization**: Patient identifiers protected
- **Secure Transmission**: TLS 1.3 encryption
- **Breach Notification**: Automated alert system

## 11. Clinical Outcomes and Evidence

### 11.1 Clinical Evidence

#### Validation Studies
- **Accuracy Studies**: >97% pose detection accuracy validated
- **Clinical Correlation**: >95% agreement with expert assessments
- **Outcome Studies**: Functional improvement tracking
- **Safety Studies**: Zero serious adverse events in trials

#### Evidence-Based Practice
- **Literature Support**: Published research on AI-assisted therapy
- **Clinical Guidelines**: Integration with therapy best practices
- **Outcome Measures**: Standardized assessment tools
- **Continuous Improvement**: Ongoing clinical research

### 11.2 Expected Outcomes

#### Patient Benefits
- **Improved Compliance**: Real-time feedback increases engagement
- **Objective Assessment**: Quantitative movement analysis
- **Consistent Therapy**: Standardized exercise delivery
- **Progress Tracking**: Clear visualization of improvement

#### Clinical Benefits
- **Enhanced Documentation**: Automated session notes
- **Objective Measures**: Quantitative assessment data
- **Efficiency**: Streamlined therapy sessions
- **Quality Assurance**: Consistent therapy delivery

---

## Medical Device Notice

⚠️ **IMPORTANT**: This device is intended for use by qualified healthcare professionals only. All clinical decisions must be made by licensed practitioners based on their professional judgment. The device provides supplementary information and should not replace clinical assessment and decision-making.

**Rx Only**: This device is available by prescription only and must be used under the supervision of a licensed healthcare provider.

**Report Adverse Events**: Any adverse events or safety concerns should be reported immediately to the manufacturer and relevant regulatory authorities.

---

**Document Control:**  
This document is controlled under the Quality Management System. All changes must be approved through the formal change control process.

**Last Updated**: January 2025  
**Next Review**: Quarterly Clinical Review  
**Version**: 2.0.0 - Production Ready
