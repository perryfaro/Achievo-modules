<?php
  class test_project extends achievoWebTestCase {

    function test_PresenceOfProjectTypeAttribute() {
      $this->loginAchievo();
      $this->get($this->achievoURL('dispatch.php?atknodetype=project.project&atkaction=edit'));
      $this->assertField("billing_type", true, "Type field must exist when performing an edit action on the project node");
    }

  }
?>