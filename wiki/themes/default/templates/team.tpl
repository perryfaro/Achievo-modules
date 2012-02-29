= Members of this team (from Achievo) = {literal}
{| border=1 {/literal}
 |- style="background-color: #FFFACD;"
 ! Name || E-mail || Skype || Internal || Direct || Cell || Job title 
 |-
 {foreach from=$data item=address}
 | [[{$address.firstname} {$address.lastname}]] 
   || {if $address.email}mailto:{$address.email}{/if} 
   || {if $address.skype_contact}<skype>{$address.skype_contact}</skype>{/if} 
   || {$address.internal_phone} 
   || {$address.phone}
   || {$address.cellular}
   || {$address.function} 
 |-
 {/foreach}
 {literal}
 |}{/literal}

Total staff: {$data|@count} people.
