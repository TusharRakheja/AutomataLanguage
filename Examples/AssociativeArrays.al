# A program to demonstrate how to implement associative arrays in Autolang.

set arrays = {{"Harvey", "Mike", "Jessica", "Donna"}, {"Specter", "Ross", "Pearson", "Paulson"}} 	# Will hold the `arrays` (sets).

set keys = {"First", "Last"}				# They keys with which we want to associate the.

map names : keys --> arrays				# Declare the association map.										

declare int i						# An index to help with the mappings.

while (i < |arrays|) & (i < |keys|)			# As long as we're within limits.
{
	under names : keys[i] --> arrays[i]		# Map the element of keys at i to arrays at i.

	let i = i + 1					# Update the index.
}
delete i						# No need for this again.												

printr names["First"]					# Print 'raw', with the quotes in every string element etc.

print  '\\n'						# We need the newline, not quotes, so normal print.

printr names["Last"]					# Print the other array.

print  '\\n'						# Prettify.

quit							# Booyeah.
