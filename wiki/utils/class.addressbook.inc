<?php

   class addressbook 
   {
     public function render($filter="", $tpl="addressbook.tpl")
     {
       $ui = atkinstance("atk.ui.atkui");

       $data = $this->_getAddresses($filter);
       return $ui->render($tpl, array("data"=>$data), "wiki");
     }

     private function _getAddresses($filter)
     {
       $emp = atkGetNode("employee.employee");
       $records = $emp->selectDb("status='active' AND userid!='administrator' ".($filter!=""?" AND ".$filter:""), "person.firstname", "", "", array("lastname", "firstname", "email", "function", "department", "phone", "internal_phone", "cellular", "skype_contact", "is_cellphone_private"));

       for ($i=0,$_i=count($records);$i<$_i;$i++)
       {
         foreach ($records[$i] as $key=>$value)
	 {
	   if (!is_array($value))
	   {
	     $records[$i][$key] = htmlentities($value);
           }
         }
	 if ($records[$i]['is_cellphone_private']==1) 
	 {
	    $records[$i]['cellular'] = '';
	 }
       }

       return $records;
     }
   }
