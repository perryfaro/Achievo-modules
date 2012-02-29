<?php

   class birthdays
   {
     public function render()
     {
       $ui = atkinstance("atk.ui.atkui");

       $data = $this->_getBirthDays();
//       return var_export($data, true);
       return $ui->render("birthdays.tpl", array("data"=>$data), "wiki");
     }

     private function _getBirthDays()
     {
       $months = array("01"=>"January", "02"=>"February", "03"=>"March", "04"=>"April", "05"=>"May", "06"=>"June", "07"=>"July", "08"=>"August", "09"=>"September", "10"=>"October", "11"=>"November", "12"=>"December");
       $emp = atkGetNode("employee.employee");
       $records = $emp->selectDb("status='active' AND userid!='administrator'", "month(person.birthdate), day(person.birthdate)", "", "", array("lastname", "firstname", "birthdate"));
       $birthdays = array();

       for ($i=0,$_i=count($records);$i<$_i;$i++)
       {
         // encoding
         foreach ($records[$i] as $key=>$value)
	 {
	   if (!is_array($value))
	   {
	     $records[$i][$key] = htmlentities($value);
           }
         }

	 // build birthdaystruct
	 if ($records[$i]["birthdate"]["month"]!="")
	 {
	   $birthdays[$months[$records[$i]["birthdate"]["month"]]][] = array("day"=>$records[$i]["birthdate"]["day"], 
	                                                                     "month"=>$records[$i]["birthdate"]["month"],
									     "year"=>$records[$i]["birthdate"]["year"],
									     "lastname"=>$records[$i]["lastname"], "firstname"=>$records[$i]["firstname"]);
	 }
       }

       return $birthdays;
     }
   }
