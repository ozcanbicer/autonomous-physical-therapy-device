# Database Documentation

This directory contains comprehensive documentation for the Autonomous Physical Therapy Device database system.

## 📊 **Database Overview**

The database is designed with **HIPAA compliance** and **IEC 62304 Class C** medical device standards in mind.

### **Key Features:**
- **HIPAA-compliant** patient data storage
- **Row Level Security (RLS)** for data protection
- **Audit trail** system for compliance
- **Performance optimization** with indexes
- **Medical device standards** compliance

## 📋 **Database Tables**

### **Core Tables:**

1. **`patients`** - Patient information (HIPAA compliant)
   - Anonymous patient identifiers
   - Medical history and consent tracking
   - Emergency contact information

2. **`treatment_sessions`** - Physical therapy sessions
   - Session metadata and performance
   - Device tracking and therapist notes
   - Exercise session details

3. **`exercise_data`** - Individual exercise performance
   - Real-time accuracy scoring
   - Movement analysis data
   - AI feedback and recommendations

4. **`device_logs`** - System operation logs
   - Error tracking and diagnostics
   - IEC 62304 compliant error codes
   - Device health monitoring

5. **`performance_metrics`** - Patient progress tracking
   - Quantitative performance measures
   - Progress analysis over time
   - Target vs. actual comparisons

6. **`ai_models`** - AI model management
   - Model versions and metadata
   - Accuracy and validation results
   - Deployment status tracking

7. **`audit_trail`** - Compliance audit system
   - All database changes tracked
   - User action logging
   - Regulatory compliance support

## 🔐 **Security Features**

### **Row Level Security (RLS):**
- **Patients:** Can only view their own data
- **Therapists:** Can manage patient data
- **System:** Can manage logs and operations
- **Admins:** Full access with audit trail

### **Data Protection:**
- All sensitive data encrypted
- Anonymous patient identifiers
- Consent status tracking
- Access control policies

## 📈 **Database Functions**

1. **`calculate_session_metrics(session_id)`**
   - Calculate performance metrics for a session
   - Returns structured metric data

2. **`get_patient_progress(patient_id, days_back)`**
   - Track patient improvement over time
   - Configurable time period analysis

3. **`get_device_health(device_id, hours_back)`**
   - Monitor device operational status
   - Health score calculation (0-100)

## 💾 **Storage Buckets**

- **`patient-data`** - HIPAA-compliant patient files
- **`exercise-videos`** - Exercise demonstration content
- **`model-files`** - AI model weights and configurations
- **`reports`** - Generated patient reports
- **`device-logs`** - Device operation log files

## 🔧 **Setup Instructions**

1. **Environment Variables:**
   ```bash
   export SUPABASE_PROJECT_REF="your-project-ref"
   export SUPABASE_ACCESS_TOKEN="your-access-token"
   ```

2. **Run Setup:**
   ```bash
   python scripts/supabase_setup.py
   ```

3. **Verify Setup:**
   - Check all tables created
   - Verify RLS policies active
   - Test database functions
   - Confirm audit trail working

## 📋 **Compliance Checklist**

### **IEC 62304 Class C Compliance:**
- ✅ Software safety classification documented
- ✅ Risk management implemented
- ✅ Audit trail system active
- ✅ Error handling and logging
- ✅ Data integrity measures

### **HIPAA Compliance:**
- ✅ Patient data encryption
- ✅ Access control implementation
- ✅ Audit logging system
- ✅ Data anonymization
- ✅ Consent tracking

### **Performance Requirements:**
- ✅ Optimized indexes for queries
- ✅ Efficient data storage design
- ✅ Real-time data processing support
- ✅ Scalable architecture

## 📊 **Monitoring and Maintenance**

### **Key Metrics to Monitor:**
- Database query performance
- Storage usage and growth
- Error rates and types
- User access patterns
- Compliance audit results

### **Regular Maintenance:**
- Index optimization
- Audit log archival
- Performance monitoring
- Security updates
- Backup verification

## 🔗 **Related Documentation**

- [API Specification](../api/api_specification.md)
- [System Architecture](../design/system_architecture.md)
- [IEC 62304 Compliance](../compliance/iec62304_compliance.md)
- [Functional Requirements](../requirements/functional_requirements.md)

---

**Note:** This database system is designed for medical device applications and follows strict regulatory compliance standards. All changes must be reviewed and approved following IEC 62304 procedures.
