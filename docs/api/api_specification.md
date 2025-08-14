# API Specification
**Autonomous Physical Therapy Device**  
**Document Version:** 1.0.0  
**Date:** 2025-01-14  
**Classification:** IEC 62304 Class C Medical Device Software  

## Document Information

| Field | Value |
|-------|--------|
| Document ID | API-SPEC-001 |
| Version | 1.0.0 |
| Author | Software Development Team |
| Reviewer | System Architecture Team |
| Approver | Technical Director |
| Classification | IEC 62304 Class C |
| Traceability | FR-SYS-004, FR-UI-001, FR-DATA-001 |

## 1. Introduction

This document defines the comprehensive API specifications for the Autonomous Physical Therapy Device, including WebSocket real-time APIs, REST APIs, and internal component interfaces.

### 1.1 API Design Principles
- **Security First**: All APIs implement comprehensive security measures
- **Real-time Capability**: Low-latency communication for medical applications
- **Medical Compliance**: HIPAA and medical device regulatory compliance
- **Reliability**: Robust error handling and recovery mechanisms
- **Versioning**: Backward compatibility and version management

### 1.2 Authentication and Security
- **TLS 1.3**: All API communications encrypted
- **JWT Tokens**: Stateless authentication with expiration
- **Role-based Access**: Different access levels for patients, providers, administrators
- **Rate Limiting**: Protection against abuse and DoS attacks
- **Audit Logging**: Complete audit trail for all API calls

## 2. WebSocket Real-time API

### 2.1 Connection Establishment

**Endpoint:** `wss://device.example.com/api/v1/realtime`

**Authentication:**
```javascript
{
  "type": "auth",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "deviceId": "DEVICE_001",
  "sessionId": "SESSION_12345"
}
```

**Response:**
```javascript
{
  "type": "auth_response",
  "status": "success",
  "userId": "USER_123",
  "permissions": ["read_session", "write_commands"],
  "serverTime": "2025-01-14T10:30:00Z"
}
```

### 2.2 Session Management

#### 2.2.1 Start Session
**Request:**
```javascript
{
  "type": "start_session",
  "sessionId": "SESSION_12345",
  "patientId": "PATIENT_456",
  "exerciseType": "shoulder_mobility",
  "duration": 1800,
  "parameters": {
    "difficulty": "beginner",
    "targetReps": 10,
    "restPeriod": 30
  }
}
```

**Response:**
```javascript
{
  "type": "session_started",
  "sessionId": "SESSION_12345",
  "status": "active",
  "startTime": "2025-01-14T10:30:00Z",
  "estimatedDuration": 1800,
  "cameraStatus": "connected",
  "aiStatus": "ready"
}
```

#### 2.2.2 End Session
**Request:**
```javascript
{
  "type": "end_session",
  "sessionId": "SESSION_12345",
  "reason": "completed",
  "summary": {
    "completedReps": 8,
    "averageForm": 0.92,
    "totalDuration": 1650
  }
}
```

**Response:**
```javascript
{
  "type": "session_ended",
  "sessionId": "SESSION_12345",
  "status": "completed",
  "endTime": "2025-01-14T11:00:00Z",
  "dataStored": true,
  "reportGenerated": true
}
```

### 2.3 Real-time Data Streaming

#### 2.3.1 Pose Data Stream
**Message Type:** `pose_data`
```javascript
{
  "type": "pose_data",
  "timestamp": "2025-01-14T10:30:15.123Z",
  "frameNumber": 12345,
  "confidence": 0.95,
  "landmarks": [
    {
      "id": 0,
      "name": "nose",
      "x": 0.5,
      "y": 0.3,
      "z": -0.1,
      "visibility": 0.98,
      "presence": 0.99
    },
    // ... 32 more landmarks
  ],
  "boundingBox": {
    "x": 0.2,
    "y": 0.1,
    "width": 0.6,
    "height": 0.8
  }
}
```

#### 2.3.2 Exercise Feedback
**Message Type:** `exercise_feedback`
```javascript
{
  "type": "exercise_feedback",
  "timestamp": "2025-01-14T10:30:15.123Z",
  "exerciseId": "shoulder_mobility_001",
  "currentRep": 5,
  "feedback": {
    "form": {
      "score": 0.88,
      "issues": ["elbow_position", "movement_speed"],
      "suggestions": [
        "Keep elbow closer to body",
        "Slow down the movement"
      ]
    },
    "range": {
      "current": 85.5,
      "target": 90.0,
      "progress": 0.95
    },
    "timing": {
      "repDuration": 3.2,
      "targetDuration": 3.0,
      "phase": "concentric"
    }
  }
}
```

#### 2.3.3 System Status Updates
**Message Type:** `system_status`
```javascript
{
  "type": "system_status",
  "timestamp": "2025-01-14T10:30:15.123Z",
  "status": "operational",
  "components": {
    "camera": {
      "status": "connected",
      "fps": 30.2,
      "latency": 12.5,
      "temperature": 42.1
    },
    "ai": {
      "status": "processing",
      "processingTime": 45.2,
      "confidence": 0.94,
      "modelVersion": "v2.1.0"
    },
    "system": {
      "cpuUsage": 35.2,
      "memoryUsage": 2048,
      "diskUsage": 45.8,
      "uptime": 86400
    }
  }
}
```

### 2.4 Command and Control

#### 2.4.1 Emergency Stop
**Request:**
```javascript
{
  "type": "emergency_stop",
  "reason": "patient_distress",
  "userId": "PROVIDER_789",
  "timestamp": "2025-01-14T10:30:15.123Z"
}
```

**Response:**
```javascript
{
  "type": "emergency_stop_executed",
  "status": "stopped",
  "responseTime": 45,
  "safetyState": "achieved",
  "timestamp": "2025-01-14T10:30:15.168Z"
}
```

#### 2.4.2 Pause/Resume Session
**Request:**
```javascript
{
  "type": "pause_session",
  "sessionId": "SESSION_12345",
  "reason": "patient_rest"
}
```

**Response:**
```javascript
{
  "type": "session_paused",
  "sessionId": "SESSION_12345",
  "pausedAt": "2025-01-14T10:30:15.123Z",
  "resumeAllowed": true
}
```

## 3. REST API Endpoints

### 3.1 Base Configuration
- **Base URL:** `https://device.example.com/api/v1`
- **Authentication:** Bearer JWT token in Authorization header
- **Content-Type:** `application/json`
- **Rate Limiting:** 1000 requests per hour per user

### 3.2 Authentication Endpoints

#### 3.2.1 Login
```http
POST /auth/login
Content-Type: application/json

{
  "username": "provider@clinic.com",
  "password": "securePassword123",
  "deviceId": "DEVICE_001",
  "role": "healthcare_provider"
}
```

**Response:**
```http
HTTP/1.1 200 OK
Content-Type: application/json

{
  "success": true,
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "refreshToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "expiresIn": 3600,
  "user": {
    "id": "USER_123",
    "role": "healthcare_provider",
    "permissions": ["read_patients", "manage_sessions"]
  }
}
```

#### 3.2.2 Refresh Token
```http
POST /auth/refresh
Content-Type: application/json

{
  "refreshToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

### 3.3 Patient Management

#### 3.3.1 Get Patients
```http
GET /patients?page=1&limit=20&search=john
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

**Response:**
```json
{
  "success": true,
  "data": {
    "patients": [
      {
        "id": "PATIENT_456",
        "anonymizedId": "P456",
        "demographics": {
          "ageGroup": "50-60",
          "gender": "M"
        },
        "condition": "shoulder_impingement",
        "treatmentPlan": "mobility_improvement",
        "lastSession": "2025-01-13T14:30:00Z",
        "progress": {
          "sessionsCompleted": 12,
          "totalSessions": 20,
          "improvementScore": 0.78
        }
      }
    ],
    "pagination": {
      "page": 1,
      "limit": 20,
      "total": 156,
      "pages": 8
    }
  }
}
```

#### 3.3.2 Create Patient
```http
POST /patients
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
Content-Type: application/json

{
  "demographics": {
    "ageGroup": "30-40",
    "gender": "F"
  },
  "condition": "knee_rehabilitation",
  "treatmentPlan": "strength_building",
  "medicalHistory": {
    "surgeries": ["ACL_repair"],
    "medications": ["ibuprofen"],
    "allergies": []
  },
  "consentGiven": true,
  "consentDate": "2025-01-14T10:00:00Z"
}
```

### 3.4 Session Management

#### 3.4.1 Get Sessions
```http
GET /sessions?patientId=PATIENT_456&startDate=2025-01-01&endDate=2025-01-14
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

**Response:**
```json
{
  "success": true,
  "data": {
    "sessions": [
      {
        "id": "SESSION_12345",
        "patientId": "PATIENT_456",
        "exerciseType": "shoulder_mobility",
        "startTime": "2025-01-14T10:30:00Z",
        "endTime": "2025-01-14T11:00:00Z",
        "duration": 1800,
        "status": "completed",
        "metrics": {
          "completedReps": 8,
          "targetReps": 10,
          "averageForm": 0.92,
          "maxRangeOfMotion": 87.5,
          "improvementFromLastSession": 0.05
        },
        "feedback": {
          "overallScore": 0.88,
          "areasOfImprovement": ["consistency", "range"],
          "positiveAspects": ["form", "effort"]
        }
      }
    ],
    "summary": {
      "totalSessions": 12,
      "averageScore": 0.85,
      "improvementTrend": "positive"
    }
  }
}
```

#### 3.4.2 Get Session Details
```http
GET /sessions/SESSION_12345
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

**Response:**
```json
{
  "success": true,
  "data": {
    "session": {
      "id": "SESSION_12345",
      "patientId": "PATIENT_456",
      "exerciseType": "shoulder_mobility",
      "startTime": "2025-01-14T10:30:00Z",
      "endTime": "2025-01-14T11:00:00Z",
      "detailedMetrics": {
        "reps": [
          {
            "repNumber": 1,
            "duration": 3.2,
            "rangeOfMotion": 82.5,
            "formScore": 0.89,
            "issues": ["slight_compensation"]
          }
        ],
        "biomechanics": {
          "jointAngles": "data/session_12345_angles.json",
          "velocityProfile": "data/session_12345_velocity.json",
          "accelerationData": "data/session_12345_acceleration.json"
        }
      },
      "aiAnalysis": {
        "modelVersion": "v2.1.0",
        "confidence": 0.94,
        "anomaliesDetected": [],
        "recommendations": [
          "Increase range of motion gradually",
          "Focus on eccentric control"
        ]
      }
    }
  }
}
```

### 3.5 Exercise Management

#### 3.5.1 Get Exercise Library
```http
GET /exercises?category=shoulder&difficulty=beginner
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

**Response:**
```json
{
  "success": true,
  "data": {
    "exercises": [
      {
        "id": "shoulder_mobility_001",
        "name": "Shoulder Forward Flexion",
        "category": "shoulder",
        "difficulty": "beginner",
        "description": "Raise arm forward and up to shoulder height",
        "instructions": [
          "Stand with feet shoulder-width apart",
          "Keep arm straight",
          "Raise arm slowly to shoulder height",
          "Hold for 2 seconds",
          "Lower slowly"
        ],
        "parameters": {
          "targetReps": 10,
          "restPeriod": 30,
          "targetRangeOfMotion": 90,
          "targetDuration": 3.0
        },
        "contraindications": [
          "acute_shoulder_pain",
          "recent_shoulder_surgery"
        ],
        "videoUrl": "https://cdn.example.com/exercises/shoulder_mobility_001.mp4",
        "thumbnailUrl": "https://cdn.example.com/exercises/shoulder_mobility_001_thumb.jpg"
      }
    ]
  }
}
```

### 3.6 Device Management

#### 3.6.1 Get Device Status
```http
GET /device/status
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

**Response:**
```json
{
  "success": true,
  "data": {
    "deviceId": "DEVICE_001",
    "status": "operational",
    "lastUpdate": "2025-01-14T10:30:00Z",
    "hardware": {
      "camera": {
        "model": "ORBBEC_Femto_Mega",
        "status": "connected",
        "firmware": "2.1.0",
        "temperature": 42.1,
        "calibrationDate": "2025-01-01T00:00:00Z"
      },
      "compute": {
        "model": "Jetson_Orin_NX",
        "cpuUsage": 35.2,
        "memoryUsage": 2048,
        "gpuUsage": 28.5,
        "temperature": 55.3
      }
    },
    "software": {
      "version": "1.0.0",
      "buildDate": "2025-01-10T12:00:00Z",
      "lastUpdateCheck": "2025-01-14T09:00:00Z",
      "updateAvailable": false
    },
    "network": {
      "connectionType": "ethernet",
      "signalStrength": 100,
      "latency": 12.5,
      "throughput": 1000
    }
  }
}
```

#### 3.6.2 Device Configuration
```http
PUT /device/config
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
Content-Type: application/json

{
  "camera": {
    "resolution": "1920x1080",
    "fps": 30,
    "enableDepth": true,
    "depthRange": {
      "min": 500,
      "max": 5000
    }
  },
  "ai": {
    "modelVersion": "v2.1.0",
    "confidenceThreshold": 0.8,
    "enableRealTime": true
  },
  "session": {
    "maxDuration": 3600,
    "autoSave": true,
    "enableRecording": false
  }
}
```

## 4. Error Handling

### 4.1 Error Response Format
```json
{
  "success": false,
  "error": {
    "code": "INVALID_SESSION",
    "message": "Session not found or expired",
    "details": {
      "sessionId": "SESSION_12345",
      "reason": "session_expired",
      "timestamp": "2025-01-14T10:30:00Z"
    },
    "traceId": "trace_abc123"
  }
}
```

### 4.2 HTTP Status Codes
- **200 OK**: Successful request
- **201 Created**: Resource created successfully
- **400 Bad Request**: Invalid request parameters
- **401 Unauthorized**: Authentication required
- **403 Forbidden**: Insufficient permissions
- **404 Not Found**: Resource not found
- **409 Conflict**: Resource conflict
- **429 Too Many Requests**: Rate limit exceeded
- **500 Internal Server Error**: Server error
- **503 Service Unavailable**: Service temporarily unavailable

### 4.3 Medical Device Specific Errors
- **MEDICAL_SAFETY_VIOLATION**: Safety protocols violated
- **DEVICE_CALIBRATION_REQUIRED**: Device requires calibration
- **PATIENT_SAFETY_ALERT**: Patient safety concern detected
- **EMERGENCY_STOP_TRIGGERED**: Emergency stop activated
- **COMPLIANCE_VIOLATION**: Regulatory compliance issue

## 5. Rate Limiting and Quotas

### 5.1 Rate Limits
- **Authentication**: 10 requests per minute
- **Real-time Data**: No limit (WebSocket)
- **REST API**: 1000 requests per hour
- **File Upload**: 100MB per hour
- **Emergency Commands**: No limit

### 5.2 Quota Management
- **Data Storage**: 10GB per patient per year
- **Session Recording**: 100 hours per month
- **API Calls**: 10,000 per day per device
- **Concurrent Sessions**: 1 per device (current limitation)

## 6. Versioning and Backward Compatibility

### 6.1 API Versioning
- **Current Version**: v1
- **Version Header**: `API-Version: v1`
- **URL Versioning**: `/api/v1/`
- **Deprecation Notice**: 6 months advance notice
- **Support Policy**: 2 versions maintained simultaneously

### 6.2 Breaking Changes Policy
- Major version increment for breaking changes
- Backward compatibility maintained for minor versions
- Deprecation warnings in response headers
- Migration guides provided for major changes

## 7. Monitoring and Analytics

### 7.1 API Metrics
- Request/response times
- Error rates by endpoint
- Authentication failures
- Rate limit violations
- Device performance metrics

### 7.2 Health Checks
```http
GET /health
```

**Response:**
```json
{
  "status": "healthy",
  "timestamp": "2025-01-14T10:30:00Z",
  "version": "1.0.0",
  "components": {
    "database": "healthy",
    "camera": "healthy",
    "ai": "healthy",
    "storage": "healthy"
  },
  "uptime": 86400
}
```

## 8. Compliance and Security

### 8.1 HIPAA Compliance
- All patient data encrypted in transit and at rest
- Audit logging for all data access
- Access controls and authentication
- Data minimization principles
- Breach notification procedures

### 8.2 Medical Device Compliance
- IEC 62304 Class C software lifecycle
- Risk management integration
- Change control procedures
- Validation and verification
- Clinical evaluation support

## 9. SDK and Client Libraries

### 9.1 JavaScript/TypeScript SDK
```typescript
import { TherapyDeviceAPI } from '@therapy-device/sdk';

const api = new TherapyDeviceAPI({
  baseUrl: 'https://device.example.com/api/v1',
  token: 'your-jwt-token'
});

// Start a session
const session = await api.sessions.start({
  patientId: 'PATIENT_456',
  exerciseType: 'shoulder_mobility'
});

// Listen for real-time updates
api.realtime.onPoseData((data) => {
  console.log('Pose data:', data);
});
```

### 9.2 Python SDK
```python
from therapy_device import TherapyDeviceAPI

api = TherapyDeviceAPI(
    base_url='https://device.example.com/api/v1',
    token='your-jwt-token'
)

# Start a session
session = api.sessions.start(
    patient_id='PATIENT_456',
    exercise_type='shoulder_mobility'
)

# Get session data
data = api.sessions.get_detailed_data(session.id)
```

## 10. Approval and Change Control

**Document Approval:**
- Technical Director: [Signature Required]
- Software Development Manager: [Signature Required]
- Quality Assurance Manager: [Signature Required]
- Medical Device Manager: [Signature Required]

**Change Control:**
All API changes must be approved through the formal change control process with impact analysis.

**Next Review Date:** 2025-07-14

---
**Document Control:**  
This document is controlled under the Quality Management System. Uncontrolled copies are for reference only.
