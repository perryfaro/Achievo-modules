<?php
  class test_employee extends achievoWebTestCase {
    
    function test_PresenceOfEmployeeTargetDeclarabilityAttribute() {
      $this->loginAchievo();
      $this->get($this->achievoURL('dispatch.php?atknodetype=employee.employee&atkaction=edit'));
      $this->assertField("billing_declarability", true, "Target declarability field must exist when performing an edit action on the employee node");
      $this->get($this->achievoURL('dispatch.php?atknodetype=employee.employee&atkaction=add'));
      $this->assertField("billing_declarability", true, "Target declarability field must exist when performing an add action on the employee node");
    }
  
  }
?>