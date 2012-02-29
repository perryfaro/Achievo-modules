function save_hours_on_billinglink(billinglink,alerttext)
{
  var saveFirst = confirm(alerttext);
  if (saveFirst == true)
  {
    // get form element
    var theformid = -1;
    
    for(var i = 0; i < document.forms.length; i++)
    {
      if (document.forms[i].name=="entryform")
      {
        theformid = i;
        break;
      }
    }
    
    if (theformid==-1)
    {
      alert("Error: could not find form");
      return;
    }
    
    // add hidden field to our form
    var myInput = document.createElement('input');
    myInput.setAttribute("type", "hidden");
    myInput.setAttribute("name", "redirect_after_save");
    myInput.setAttribute("value", billinglink);
    
    document.forms[theformid].appendChild(myInput);
    
    // submit form
    document.forms[theformid].submit();
  }
  else
  {
    document.location = billinglink;
  }
}