{literal}
{| border=1 {/literal}
 |- style="background-color: #FFFACD;"
 ! Name || E-mail || Skype || Internal || Direct || Cell || Job title || Team
 |-
 {foreach from=$data item=address}
 | [[{$address.firstname} {$address.lastname}]] 
   || {if $address.email}mailto:{$address.email}{/if} 
   || {if $address.skype_contact}<skype>{$address.skype_contact}</skype>{/if}
   || {$address.internal_phone}  
   || {$address.phone}
   || {$address.cellular}
   || {$address.function} 
   || {if $address.department.name}[[{$address.department.name}]]{/if} 
 |-
 {/foreach}
 {literal}
 |}{/literal}

Total staff: {$data|@count} people.
