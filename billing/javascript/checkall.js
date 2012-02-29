/**
 *
 * Billing module check-all Javascripts
 *
 *
 * @author Ivo Jansch <ivo@ibuildings.nl>
 * @version $Revision: 2686 $
 *
 * $Id: checkall.js 2686 2004-01-20 21:49:32Z ivo $
 *
 */
  

function bill_markAll()
{
  bill_checkAll("mark");
}

function bill_markNone()
{
  bill_checkNone("mark");  
}

function bill_mark(id)
{
  el_mark = document.getElementById('mark_'+id);
  el_bill = document.getElementById('bill_'+id);
  if (el_bill.checked) el_mark.checked = true;
}

function bill_billAll()
{
  bill_checkAll("bill");
  bill_markAll(); // What we bill, we also mark.
}

function bill_billNone()
{
  bill_checkNone("bill");
}

function bill_checkAll(fieldname)
{    
  with (document.billform)
  {
    for(i=0; i<elements.length; i++)
    {
      if (elements[i].name.substr(0,fieldname.length)==fieldname)
      {
        elements[i].checked = true;
      }
    }
  }
}

function bill_checkNone(fieldname)
{    
  with (document.billform)
  {
    for(i=0; i<elements.length; i++)
    {
      if (elements[i].name.substr(0,fieldname.length)==fieldname)
      {
        elements[i].checked = false;
      }
    }
  }
}

/*
function bill_checkInvert(fieldname)
{    
  with (document.billform)
  {
    for(i=0; i<elements.length; i++)
    {
      if (elements[i].name.substr(0,fieldname.length)==fieldname)
      {
        elements[i].checked = !elements[i].checked;
      }
    }
  }
}
*/