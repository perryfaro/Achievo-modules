<?php
   define('FPDF_FONTPATH','font/');
   require('fpdf.php');
   
   $pdf=new FPDF();
   $pdf->Open();
   $pdf->AddPage();
   $pdf->SetFont('Arial','B',16);
   $pdf->Cell(40,10,'Hello World!');
   $pdf->SetY(150);
   $pdf->Image('ssplogo.jpg',10,10,50,0);
   $pdf->Output();
?>
