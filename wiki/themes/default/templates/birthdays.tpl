{foreach from=$data item=birthdays key=month}
=={$month}==
{foreach from=$birthdays item=birthday}
* {$birthday.day}-{$birthday.month}-{$birthday.year} : [[{$birthday.firstname} {$birthday.lastname}]] 
{/foreach}
{/foreach}
