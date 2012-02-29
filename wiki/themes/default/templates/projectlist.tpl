= Projects worked on in the past week = 
{foreach from=$data item=project}
* [[{$project.wikilink}|{$project.name}]] 
{/foreach}

Total projects in past week: {$data|@count}.
