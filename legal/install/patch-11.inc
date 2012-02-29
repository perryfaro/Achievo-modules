<?php

  $setup->installNode("docmanager.documenttype");
  $setup->installNode("project.project");

  $projectnode = &atkGetNode("project.project");
  $projects = $projectnode->selectDb("", "", "", "", array("name", "legal_casenumber"));

  $legalmod = &getModule("legal");
  $defaultdir = "";
  $legaldir = atkconfig("documentpath", "documents/");

  $fails = array();
  for($i=0,$_i=count($projects); $i<$_i; $i++)
  {
    $projects[$i]["olddir"] = $legalmod->getCustomProjectDocumentDir($projects[$i]["legal_casenumber"], $defaultdir, true);
    $projects[$i]["newdir"] = $legalmod->getCustomProjectDocumentDir($projects[$i]["legal_casenumber"], $defaultdir, false);
    if (!file_exists($legaldir.$projects[$i]["olddir"]))
      $fails[] = 'Dossier: '.$projects[$i]["legal_casenumber"].' ('. $projects[$i]["name"].')   Map: '.$projects[$i]["olddir"];
  }

  // Halt if any folders do not exist
  if (count($fails)>0)
  {
    echo "Following folders do not exist<br><br>";
    foreach($fails as $fail) echo "$fail<br>";
    die("<br>Halted.<br>");
  }

  // Check for improperly translated subfolders of the project dir
  $legaldoctypes = $legalmod->getLegalDocTypes();

  // Make the subfolders
  foreach($projects as $project)
  {
    $olddir = $legaldir.$project["olddir"];
    foreach ($legaldoctypes as $legaldoctype)
    {
      $typefolder = $olddir.atktext($legaldoctype, "legal", "", atkconfig("language"));
      if (!file_exists($typefolder))
      {
        $wrongtypefolder = $olddir.atktext($legaldoctype, "legal", "", "en");
        if (!file_exists($wrongtypefolder))
          $fails[] = "NOT FOUND ".$typefolder;
        else if (!rename($wrongtypefolder, $typefolder))
          $fails[] = "ERROR RENAMING '$wrongtypefolder' to '$typefolder'";
      }
    }
  }

  // Halt if any folders do not exist
  if (count($fails)>0)
  {
    echo "Following errors ocurred<br><br>";
    foreach($fails as $fail) echo "$fail<br>";
    die("<br>Halted.<br>");
  }


  // Make the subfolders
  foreach($projects as $project)
  {
    $olddircomps = explode('/',$project["olddir"]);
    $oldcasetypedir = $olddircomps[0].'/'.$olddircomps[1];
    $newdircomps = explode('/',$project["newdir"]);
    $newcasetypedir = $newdircomps[0].'/'.$newdircomps[1];
    $oldprojdir = $newdircomps[0].'/'.$newdircomps[1].'/'.$olddircomps[2].'/'.$olddircomps[3];
    $newprojdir = $project["newdir"];

    // Try renaming until the casetype
    if (!file_exists($legaldir.$newcasetypedir))
      if (!rename($legaldir.$oldcasetypedir, $legaldir.$newcasetypedir))
        $fails[] = "ERROR RENAMING '$legaldir$oldcasetypedir' to '$legaldir$newcasetypedir'";


    // Now try renaming with the project number
    if (!file_exists($legaldir.$newprojdir))
      if (!rename($legaldir.$oldprojdir, $legaldir.$newprojdir))
        $fails[] = "ERROR RENAMING '$legaldir$oldprojdir' to '$legaldir$newprojdir'";
  }

  // Halt if any folders do not exist
  if (count($fails)>0)
  {
    echo "Following errors ocurred<br><br>";
    foreach($fails as $fail) echo "$fail<br>";
    die("<br>Halted.<br>");
  }

?>