<?php
/*  prints the bill in PDF, the data is read
*   from the db
*   B.Graf (2.7.03)
*/
    include_once(moduleDir("billing")."pdfbill.php");
    
    $bill_line_node = &getNode("billing.bill_line");
    global $bill_id;
    $billid = $_REQUEST["billid"];

    $currency_symbol = $bill_line_node->get_currency_symbol($billid);
    
    $pdf=new pdfbill();
    $pdf->Open();
    $pdf->AddPage();
    $pdf->AliasNbPages();
    
    //get length of currency symbol
    $amountlength = 40;
    $symbollength = intval($pdf->GetStringWidth($currency_symbol))+2;
    $taxlength = intval($pdf->GetStringWidth(atkconfig("tax_rate")))+2;
    $linelength = 190 - $amountlength - $symbollength - $taxlength;
    

    //get billinfos
    $sql = "SELECT create_date, expire_date, contactperson, customer_info, bill_nr ";
    $sql .= "FROM bill where id = ".$billid;

    $db = &atkGetDb();
    $billinfo=$db->getrows($sql);
    
    //here you can print your company address onto the bill

    //startpoint of bill
    $pdf->SetY(40);
    
    //prints address, date, billnumber on first side of bill
    $pdf->SetFont('Arial','B',16);
    $pdf->Cell(50,10,text("outputbill_bill"),0,1);

    $pdf->SetFont('Arial','',14);

    $customerinfo = ($billinfo[0]["customer_info"]);
    $pdf->Cell(120, 10, "", 0,0);
    $pdf->MultiCell(70,10,$customerinfo,0,1);
    
    $contact = $billinfo[0]["contactperson"];
    
    if (isset($contact)){
      $pdf->Cell(120, 10, "",0,0);
      $pdf->Cell(70,10,text("outputbill_contact")." ".$contact,0,1);
    }
    $pdf->SetY(100);

    $pdf->SetFont('Arial','',12);
    $pdf->Cell(100,8,text("outputbill_billnr")." ".$billinfo[0]["bill_nr"],0,1);
    
    //prints the bill date
    $year = substr($billinfo[0]["create_date"], 0, 4);
    $month = substr($billinfo[0]["create_date"], 5, 2);
    $day = substr($billinfo[0]["create_date"], 8, 2);
    $monthfinal = date("M", mktime(0,0,0,$month,1,2000));

    $pdf->Cell(100,8,text("outputbill_billdate")." ".$day." ".$monthfinal." ".$year,0,1);

    //calculates and outputs the expiring date of the bill
    $year = substr($billinfo[0]["expire_date"], 0, 4);
    $month = substr($billinfo[0]["expire_date"], 5, 2);
    $day = substr($billinfo[0]["expire_date"], 8, 2);
    $monthfinal = date("M", mktime(0,0,0,$month,1,2000));

    $pdf->Cell(100,8,text("outputbill_billexpiredate")." ".$day." ".$monthfinal." ".$year,0,1);

    //prints column header of billlines
    $pdf->SetFont('Arial','B',14);
    $pdf->SetFillColor(200);
    $pdf->Cell(140,10,text("outputbill_description"),1,0,'L',1);
    $pdf->Cell(20,10,atkconfig("tax_symbol")."%",1,0,'C',1);
    $pdf->Cell(30,10,text("outputbill_amount"),1,1,'C',1);
    
    $pdf->SetFont('Arial','',12);

    $sql = "SELECT
              id,
              billid,
              fixed_billed,
              calcoption,
              description_final
            FROM bill_line
            WHERE billid = ".$billid;

    $bill_lineinfo=$db->getrows($sql);

    //prints fixed lines
    for ($x=0;$x<count($bill_lineinfo);$x++)
    {
      $billlineid[] = $bill_lineinfo[$x]["id"];
    }

    $sql = "SELECT
		          type,
		          amount,
		          bill_line_id,
		          apply_on
		        FROM discount
		        WHERE
		        bill_line_id IN (".implode(",",$billlineid).") ";

	  $discountinfo=$db->getrows($sql);

    for ($x=0;$x<count($bill_lineinfo);$x++)
    {
      if ($bill_lineinfo[$x]["calcoption"] == "fixed")
      {
        $bill_line_amount = $bill_lineinfo[$x]["fixed_billed"];
        for ($i=0;$i<count($discountinfo);$i++)
        {
          if ($discountinfo[$i]["apply_on"] == $bill_lineinfo[$x]["id"])
          {
            if ($discountinfo[$i]["type"] == 1)
            {
              //discount is an amount
              $bill_line_amount = $bill_line_amount - $discountinfo[$i]["amount"];
            }
            else
            {
              //discount is a percentage
              $bill_line_amount = $bill_line_amount - ($bill_line_amount/100)*$discountinfo[$i]["amount"];
            }
          }
        }
        $pdf->NewPage();
        $pdf->Cell(140,10,$bill_lineinfo[$x]["description_final"],0,0);
        $pdf->Cell(20,10,atkconfig("tax_rate"),0,0,'C');
        $pdf->Cell(30,10,$currency_symbol." ".number_format($bill_line_amount, 2,",","."),0,1,'R');
        $subtotal += $bill_line_amount;
        $lines++;
      }

    }

    for ($x=0;$x<count($bill_lineinfo);$x++)
    {
      if ($bill_lineinfo[$x]["calcoption"] == "calc")
      {
        $bill_line_amount = $bill_lineinfo[$x]["fixed_billed"];
        for ($i=0;$i<count($discountinfo);$i++)
        {
          if ($discountinfo[$i]["apply_on"] == $bill_lineinfo[$x]["id"])
          {
            if ($discountinfo[$i]["type"] == 1)
            {
              //discount is an amount
              $bill_line_amount = $bill_line_amount - $discountinfo[$i]["amount"];
            }
            else
            {
              //discount is a percentage
              $bill_line_amount = $bill_line_amount - ($bill_line_amount/100)*$discountinfo[$i]["amount"];
            }
          }
        }
        $pdf->NewPage();
        $pdf->Cell(140,10,$bill_lineinfo[$x]["description_final"],0,0);
        $pdf->Cell(20,10,atkconfig("tax_rate"),0,0,'C');
        $pdf->Cell(30,10,$currency_symbol." ".number_format($bill_line_amount, 2,",","."),0,1,'R');
        $subtotal += $bill_line_amount;
        $lines++;
      }
    }

    for ($x=0;$x<count($bill_lineinfo);$x++)
    {
      if ($bill_lineinfo[$x]["calcoption"] == "costs")
      {
        $bill_line_amount = $bill_lineinfo[$x]["fixed_billed"];
        $pdf->NewPage();
        $pdf->Cell(140,10,$bill_lineinfo[$x]["description_final"],0,0);
        $pdf->Cell(20,10,atkconfig("tax_rate"),0,0,'C');
        $pdf->Cell(30,10,$currency_symbol." ".number_format($bill_line_amount, 2,",","."),0,1,'R');
        $subtotal += $bill_line_amount;
        $lines++;
      }
    }

    for ($x=0;$x<=count($discountinfo);$x++)
    {
      if ($discountinfo[$x]["apply_on"] == "bill")
      {
        //entire bill discount: %
        if ($discountinfo[$x]["type"] == 2)
        {
          $lines++;
          $pdf->NewPage();
          $entire = ($subtotal / 100) * $discountinfo[$x]["amount"];
          
          $pdf->Cell(140,10,$discountinfo[$x]["amount"]."%"." ".text("outputbill_entirediscount"),0,0);
          $pdf->Cell(50,10,$currency_symbol." ".round($entire,2)." -",0,1,'R');
          $subtotal -= $entire;
        }
      }
    }

    for ($x=0;$x<=count($discountinfo);$x++)
    {
      if ($discountinfo[$x]["apply_on"] == "bill")
      {
        //entire bill discount: amount
        if ($discountinfo[$x]["type"] == 1)
        {
          $lines++;
          $pdf->NewPage();
          $pdf->Cell(140,10,$currency_symbol." ".number_format($discountinfo[$x]["amount"], 2,",",".")." ".text("outputbill_entirediscount"),0,0);
          $pdf->Cell(50,10,$currency_symbol." ".number_format(round($discountinfo[$x]["amount"],2), 2,",",".")." -",0,1,'R');
          $subtotal -= $discountinfo[$x]["amount"];
        }
      }
    }

    //go to the end of the page to print out total amount
    $pdf->NewPage();
    $pdf->setY(-50);

    //calculates taxes
    $taxtotal = $this->calculate_tax($subtotal);
    if (atkconfig("tax_included")) $totalvalue = $subtotal;
    else $totalvalue = $taxtotal + $subtotal;
    
    //calculate length of different strings
    $taxlength = 50;
    $totallength = intval($pdf->GetStringWidth($totalvalue))+5;
    $restlength = 190 - $taxlength - $totallength - $symbollength;
    
    
    $pdf->Cell($restlength,8,'',0,0);
    $pdf->Cell($taxlength,8,text(atkconfig("tax_included") ? "outputbill_totalincl" : "outputbill_totalexcl").atkconfig("tax_symbol"),0,0);
    $pdf->Cell($symbollength,8,$currency_symbol,0,0);
    $pdf->Cell($totallength,8,number_format(round($subtotal,2), 2,",","."),0,1,'R');

    $pdf->Cell($restlength,8,'',0,0);
    $pdf->Cell($taxlength,8,atkconfig("tax_symbol")." ".atkconfig("tax_rate")."%",0,0);
    $pdf->Cell($symbollength,8,$currency_symbol,'B',0);
    $pdf->Cell($totallength,8,number_format(round($taxtotal,2), 2,",","."),'B',1,'R');

    $pdf->Cell($restlength,8,'',0,0);
    $pdf->Cell($taxlength,8,text("outputbill_totalpay"),0,0);
    $pdf->Cell($symbollength,8,$currency_symbol,'B',0);
    $pdf->Cell($totallength,8,number_format(round($totalvalue,2), 2,",","."),'B',1,'R');

    //underline final amount
    $pdf->SetFillColor(0);
    $pdf->Cell($restlength+$taxlength,1,'',0,0);
    $pdf->Cell($totallength+$symbollength,1,'',1,1,'C',1);

    $pdf->Output();
?>
