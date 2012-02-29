<?php

  $modifiers[] = "employee.employee";


  class mod_yubikey extends achievoModule
  {
    function employee_employee_modifier(&$node)
    {
      $node->add(new atkAttribute("yubikey", AF_HIDE_LIST, 12));
    }
  }

?>
