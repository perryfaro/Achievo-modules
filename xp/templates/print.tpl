<div class="maincontainer">

<h1>{$labels.user_story} {$values.id}</h1>
<br>

<table>
	<tr>
		<td>
			<b>{$labels.project}:</b> {$values.project.name}<br>
			<b>{$labels.title}:</b> {$values.title}<br>
			<b>{$labels.submitted_by}:</b> {$values.submitted_by.firstname} {$values.submitted_by.lastname}<br>
		</td>
		<td>
			<b>{$labels.estimate}:</b><br>
			<div class="estimate">{$values.estimate}</div>
		</td>
		<td>
			<b>{$labels.submitted_on}:</b> {$values.submitted_on}<br>
			<b>{$labels.accepted_on}:</b> {$values.accepted_on}<br>
			<b>{$labels.planned_on}:</b> {$values.planned_on}<br>
			<b>{$labels.implemented_on}:</b> {$values.implemented_on}<br>
		</td>
	</tr>
</table>
<br>

<table>
	<tr>
		<td>
			<b>{$labels.story}:</b><br>
		</td>
		<td class="text">
{$values.story}<br>
		</td>
	</tr>
	<tr>
		<td>
			<b>{$labels.comments}:</b><br>
		</td>
		<td class="text">
{$values.comments}<br>
		</td>
	</tr>
</table>

<table class="borderlesstable">
<tr>
<td class="backlink">
	{$backlink}
</td>
<td class="printtime">
	{$printtime}
</td>
</tr>
</table>

</div>