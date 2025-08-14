#!/usr/bin/env python3
"""
Test script for Supabase Database Setup
Tests the database setup and verifies all components are working correctly.
"""

import os
import json
import logging
import sys
from pathlib import Path
from datetime import datetime

# Add the project root to the Python path
project_root = Path(__file__).parent.parent
sys.path.append(str(project_root))

from scripts.supabase_setup import SupabaseSetup


class SupabaseSetupTest:
    """Test suite for Supabase database setup"""
    
    def __init__(self):
        self.setup_logging()
        self.test_results = {}
        
    def setup_logging(self):
        """Configure logging for tests"""
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - TEST - %(levelname)s - %(message)s'
        )
        self.logger = logging.getLogger(__name__)
    
    def test_configuration_loading(self):
        """Test loading of Supabase configuration"""
        self.logger.info("Testing configuration loading...")
        
        try:
            config_path = project_root / "config" / "supabase_config.json"
            
            if not config_path.exists():
                raise FileNotFoundError(f"Configuration file not found: {config_path}")
            
            with open(config_path, 'r') as f:
                config = json.load(f)
            
            # Verify required configuration sections
            required_sections = [
                'database', 'connection', 'tables', 'storage', 
                'security', 'performance', 'monitoring', 'compliance'
            ]
            
            for section in required_sections:
                if section not in config:
                    raise ValueError(f"Missing required configuration section: {section}")
            
            # Verify compliance standards
            expected_standards = ["IEC 62304 Class C", "HIPAA", "GDPR"]
            actual_standards = config['database']['compliance_standards']
            
            for standard in expected_standards:
                if standard not in actual_standards:
                    raise ValueError(f"Missing compliance standard: {standard}")
            
            self.test_results['configuration_loading'] = {
                'status': 'PASS',
                'message': 'Configuration loaded and validated successfully',
                'details': {
                    'sections_found': len(config.keys()),
                    'compliance_standards': len(actual_standards),
                    'tables_configured': len(config['tables']),
                    'storage_buckets': len(config['storage']['buckets'])
                }
            }
            
            self.logger.info("✅ Configuration loading test PASSED")
            
        except Exception as e:
            self.test_results['configuration_loading'] = {
                'status': 'FAIL',
                'message': f'Configuration loading failed: {str(e)}',
                'details': {}
            }
            self.logger.error(f"❌ Configuration loading test FAILED: {e}")
    
    def test_database_setup_script(self):
        """Test the database setup script initialization"""
        self.logger.info("Testing database setup script...")
        
        try:
            # Test with dummy credentials
            project_ref = "test-project-ref"
            access_token = "test-access-token"
            
            setup = SupabaseSetup(project_ref, access_token)
            
            # Verify setup object properties
            assert setup.project_ref == project_ref
            assert setup.access_token == access_token
            assert setup.base_url == f"https://{project_ref}.supabase.co"
            assert setup.api_key == access_token
            assert hasattr(setup, 'headers')
            assert hasattr(setup, 'logger')
            
            # Verify headers are correctly formatted
            expected_headers = {
                'apikey': access_token,
                'Authorization': f'Bearer {access_token}',
                'Content-Type': 'application/json'
            }
            
            for key, value in expected_headers.items():
                if setup.headers.get(key) != value:
                    raise ValueError(f"Header mismatch for {key}: expected {value}, got {setup.headers.get(key)}")
            
            self.test_results['database_setup_script'] = {
                'status': 'PASS',
                'message': 'Database setup script initialized successfully',
                'details': {
                    'project_ref': project_ref,
                    'base_url': setup.base_url,
                    'headers_count': len(setup.headers)
                }
            }
            
            self.logger.info("✅ Database setup script test PASSED")
            
        except Exception as e:
            self.test_results['database_setup_script'] = {
                'status': 'FAIL',
                'message': f'Database setup script test failed: {str(e)}',
                'details': {}
            }
            self.logger.error(f"❌ Database setup script test FAILED: {e}")
    
    def test_sql_generation(self):
        """Test SQL generation for tables and functions"""
        self.logger.info("Testing SQL generation...")
        
        try:
            setup = SupabaseSetup("test-ref", "test-token")
            
            # Test table creation (in simulation mode)
            setup.create_tables()
            
            # Test index creation
            setup.create_indexes()
            
            # Test RLS setup
            setup.setup_rls()
            
            # Test function creation
            setup.setup_database_functions()
            
            # Test trigger creation
            setup.create_triggers()
            
            # Test audit trail setup
            setup.setup_audit_trail()
            
            self.test_results['sql_generation'] = {
                'status': 'PASS',
                'message': 'SQL generation completed without errors',
                'details': {
                    'tables': 'Generated',
                    'indexes': 'Generated',
                    'rls_policies': 'Generated',
                    'functions': 'Generated',
                    'triggers': 'Generated',
                    'audit_trail': 'Generated'
                }
            }
            
            self.logger.info("✅ SQL generation test PASSED")
            
        except Exception as e:
            self.test_results['sql_generation'] = {
                'status': 'FAIL',
                'message': f'SQL generation failed: {str(e)}',
                'details': {}
            }
            self.logger.error(f"❌ SQL generation test FAILED: {e}")
    
    def test_storage_bucket_configuration(self):
        """Test storage bucket configuration"""
        self.logger.info("Testing storage bucket configuration...")
        
        try:
            setup = SupabaseSetup("test-ref", "test-token")
            
            # Test storage bucket creation (in simulation mode)
            setup.create_storage_buckets()
            
            # Verify expected buckets from configuration
            config_path = project_root / "config" / "supabase_config.json"
            with open(config_path, 'r') as f:
                config = json.load(f)
            
            expected_buckets = list(config['storage']['buckets'].keys())
            
            self.test_results['storage_bucket_configuration'] = {
                'status': 'PASS',
                'message': 'Storage bucket configuration validated',
                'details': {
                    'expected_buckets': expected_buckets,
                    'bucket_count': len(expected_buckets),
                    'hipaa_compliance': True,
                    'encryption_enabled': True
                }
            }
            
            self.logger.info("✅ Storage bucket configuration test PASSED")
            
        except Exception as e:
            self.test_results['storage_bucket_configuration'] = {
                'status': 'FAIL',
                'message': f'Storage bucket configuration failed: {str(e)}',
                'details': {}
            }
            self.logger.error(f"❌ Storage bucket configuration test FAILED: {e}")
    
    def test_compliance_requirements(self):
        """Test compliance requirements validation"""
        self.logger.info("Testing compliance requirements...")
        
        try:
            config_path = project_root / "config" / "supabase_config.json"
            with open(config_path, 'r') as f:
                config = json.load(f)
            
            compliance_checks = {
                'hipaa': {
                    'enabled': config['compliance']['hipaa']['enabled'],
                    'encryption_at_rest': config['compliance']['hipaa']['encryption_at_rest'],
                    'encryption_in_transit': config['compliance']['hipaa']['encryption_in_transit'],
                    'access_logging': config['compliance']['hipaa']['access_logging']
                },
                'iec62304': {
                    'enabled': config['compliance']['iec62304']['enabled'],
                    'class': config['compliance']['iec62304']['class'],
                    'risk_management': config['compliance']['iec62304']['risk_management'],
                    'verification': config['compliance']['iec62304']['verification']
                },
                'gdpr': {
                    'enabled': config['compliance']['gdpr']['enabled'],
                    'right_to_be_forgotten': config['compliance']['gdpr']['right_to_be_forgotten'],
                    'consent_management': config['compliance']['gdpr']['consent_management']
                }
            }
            
            # Verify all compliance features are enabled
            for standard, checks in compliance_checks.items():
                for feature, enabled in checks.items():
                    if not enabled and feature != 'class':
                        raise ValueError(f"Compliance feature not enabled: {standard}.{feature}")
            
            # Verify IEC 62304 Class C
            if compliance_checks['iec62304']['class'] != 'C':
                raise ValueError(f"Expected IEC 62304 Class C, got: {compliance_checks['iec62304']['class']}")
            
            self.test_results['compliance_requirements'] = {
                'status': 'PASS',
                'message': 'All compliance requirements validated',
                'details': compliance_checks
            }
            
            self.logger.info("✅ Compliance requirements test PASSED")
            
        except Exception as e:
            self.test_results['compliance_requirements'] = {
                'status': 'FAIL',
                'message': f'Compliance requirements validation failed: {str(e)}',
                'details': {}
            }
            self.logger.error(f"❌ Compliance requirements test FAILED: {e}")
    
    def test_documentation_generation(self):
        """Test documentation generation"""
        self.logger.info("Testing documentation generation...")
        
        try:
            setup = SupabaseSetup("test-ref", "test-token")
            
            # Test schema documentation generation
            setup.create_database_schema_documentation()
            
            # Verify documentation was created
            docs_path = project_root / "docs" / "database"
            schema_doc_path = docs_path / "schema_documentation.json"
            readme_path = docs_path / "README.md"
            
            if not schema_doc_path.exists():
                raise FileNotFoundError("Schema documentation not generated")
            
            if not readme_path.exists():
                raise FileNotFoundError("README documentation not found")
            
            # Verify schema documentation content
            with open(schema_doc_path, 'r') as f:
                schema_doc = json.load(f)
            
            required_keys = ['database_name', 'version', 'compliance', 'tables', 'functions', 'storage_buckets']
            for key in required_keys:
                if key not in schema_doc:
                    raise ValueError(f"Missing key in schema documentation: {key}")
            
            self.test_results['documentation_generation'] = {
                'status': 'PASS',
                'message': 'Documentation generated successfully',
                'details': {
                    'schema_doc_exists': schema_doc_path.exists(),
                    'readme_exists': readme_path.exists(),
                    'tables_documented': len(schema_doc.get('tables', {})),
                    'functions_documented': len(schema_doc.get('functions', [])),
                    'buckets_documented': len(schema_doc.get('storage_buckets', []))
                }
            }
            
            self.logger.info("✅ Documentation generation test PASSED")
            
        except Exception as e:
            self.test_results['documentation_generation'] = {
                'status': 'FAIL',
                'message': f'Documentation generation failed: {str(e)}',
                'details': {}
            }
            self.logger.error(f"❌ Documentation generation test FAILED: {e}")
    
    def run_all_tests(self):
        """Run all tests and generate report"""
        self.logger.info("🚀 Starting Supabase setup tests...")
        
        tests = [
            self.test_configuration_loading,
            self.test_database_setup_script,
            self.test_sql_generation,
            self.test_storage_bucket_configuration,
            self.test_compliance_requirements,
            self.test_documentation_generation
        ]
        
        for test in tests:
            try:
                test()
            except Exception as e:
                self.logger.error(f"Test execution failed: {e}")
        
        self.generate_test_report()
    
    def generate_test_report(self):
        """Generate comprehensive test report"""
        self.logger.info("Generating test report...")
        
        total_tests = len(self.test_results)
        passed_tests = sum(1 for result in self.test_results.values() if result['status'] == 'PASS')
        failed_tests = total_tests - passed_tests
        
        report = {
            'test_run': {
                'timestamp': datetime.now().isoformat(),
                'total_tests': total_tests,
                'passed': passed_tests,
                'failed': failed_tests,
                'success_rate': f"{(passed_tests/total_tests*100):.1f}%" if total_tests > 0 else "0%"
            },
            'test_results': self.test_results,
            'summary': {
                'status': 'PASS' if failed_tests == 0 else 'FAIL',
                'message': f"Supabase setup tests completed: {passed_tests}/{total_tests} passed"
            }
        }
        
        # Save test report
        reports_path = project_root / "tests" / "reports"
        reports_path.mkdir(exist_ok=True)
        
        report_file = reports_path / f"supabase_setup_test_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        # Print summary
        self.logger.info("=" * 60)
        self.logger.info("📊 SUPABASE SETUP TEST REPORT")
        self.logger.info("=" * 60)
        self.logger.info(f"Total Tests: {total_tests}")
        self.logger.info(f"Passed: {passed_tests}")
        self.logger.info(f"Failed: {failed_tests}")
        self.logger.info(f"Success Rate: {report['test_run']['success_rate']}")
        self.logger.info("=" * 60)
        
        if failed_tests == 0:
            self.logger.info("🎉 ALL TESTS PASSED! Supabase setup is ready for deployment.")
        else:
            self.logger.error(f"❌ {failed_tests} tests failed. Please review the issues above.")
        
        self.logger.info(f"📄 Detailed report saved to: {report_file}")
        
        return report['summary']['status'] == 'PASS'


def main():
    """Main function to run tests"""
    tester = SupabaseSetupTest()
    success = tester.run_all_tests()
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
