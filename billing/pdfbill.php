<?php
/*  Class pdfbill, defines the pathes for
*   the fonts and for the fpdf class.
*   This class is inherited from the fpdf class.
*   Here you can design the header and footer of
*   the bills. Those are the same on every page!
*   For command description see www.fpdf.org
*   B.Graf (2.7.03)
*/

define('FPDF_FONTPATH','font/');
require('fpdf151/fpdf.php');

  class pdfbill extends FPDF {

   /*   function test (){
        $this->FPDF();
      }*/

      //Page header, this is where you define the corporate identity (comes on EVERY page)
      function Header()
      {
          //Arial bold 15
          $this->SetFont('Arial','B',15);
          //prints corp name, adress etc.
          $this->Cell(0,8, 'AnyCorp Inc.','TLR',1);
          //Arial 12
          $this->SetFont('Arial','',12);
          $this->Cell(0,8,'Sample Road 15, 8888 Anytown','LR',1);
          $this->Cell(0,8,'Fon: XXX XXX XX XX, http://www.anycorp.com','BLR',0);
          $this->Ln(20);
      }

      //Page footer, sets pagenumbers
      function Footer()
      {
          //Position at 1.5 cm from bottom
          $this->SetY(-15);
          //Arial italic 8
          $this->SetFont('Arial','I',8);
          //Page number and page amount
          $this->Cell(0,10, text("bill_page")." ".$this->PageNo().'/{nb}',0,0,'C');
      }

      //checks if there is sufficient room for next line
      //and opens a new page if there isn't
      function NewPage()
      {
         $ypos = $this->GetY();
         if ($ypos > 230){
            $this->AddPage();
         }
      }
    }
?>
