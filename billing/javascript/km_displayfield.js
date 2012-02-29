/**
 * Sees whether or not to display the km field based on
 * the expensetype
 */

function maybe_display_kmfield()
{
  var selectBox = document.getElementById('expensetype_id');
  var selectedValue = "";
  
  // Get the selected value from the selectbox or the hidden field
  if (selectBox.type == "hidden")
  {
    selectedValue = selectBox.value;  
  }
  else
  {
    for(i = 0; i < selectBox.options.length; i++)
    {
      if (selectBox.options[i].selected)
      {
        selectedValue = selectBox.options[i].value;
        break;
      }
    }
  }

  // If no selected value (something went wrong, or there are no expensetypes)
  // bail out (no need to display km field)
  if (selectedValue == "") return;
  
  // Compare selected value with km-fields and descide whether or not to display
  // the kilometer field
  var doDisplay = 0;
  
  if (window.billing_dropdown_km_field != null)
  {
    for(var j = 0; j < window.billing_dropdown_km_field.length; j++)
    {
      if (window.billing_dropdown_km_field[j] == selectedValue)
      {
        doDisplay = 1;
        break;
      }
    }
  }

  // Display and hide the 'amount' and 'km' field accordingly
  var el = document.getElementById('ar_km');
  var km = document.getElementById('ar_kmamount');
  var other_el = document.getElementById('ar_amount');
  var withvat = document.getElementById('ar_amount_with_vat');
  var vat = document.getElementById('ar_vat');

  if (doDisplay)
  {
    other_el.style.display="none";
    withvat.style.display="none";
    vat.style.display="none";
    el.style.display="";
    km.style.display="";
  }
  else
  {
    other_el.style.display="";
    withvat.style.display="";
    vat.style.display="";
    el.style.display="none";
    km.style.display="none";
  }
}

// Register given value in our array that contains all km-expensetypes
function register_km_option(selectboxvalue)
{
  if (window.billing_dropdown_km_field == null)
    window.billing_dropdown_km_field = new Array();
    
  window.billing_dropdown_km_field.unshift(selectboxvalue);
}