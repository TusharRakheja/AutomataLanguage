# Sample Prog.

map testmap : {1} -> {True}		 # Initialize a map with the given domain and range.
set A = {testmap, "\(\"Foo\"\)", 3}      # Initialize a set (all braces, parentheses, brackets, quotes, backslashes, and escape characters, must have '\\' as prefix.
declare int i				 # Primitves don't have to be explicitly initialized (default 0 for int).
while i < |A|				 # "While i < cardinality of set A" (Note: The opening brace 'must' be on a new line).
{					 
	if (i == 1)			 # Parentheses are optional. This would work without them also 
	{				 # Here too, the brace HAS to open on a new line.
		print A[i]		 # Print the i'th element of A.
		print '\n'		 # Print (processed) character (two delimiting '\' needed).
	}				 # If preceded by an expression (not a command, like 'quit' or 'declare'), the closing brace must also be on a new line. 
	else { } 			 # (Optional 'else') The else opening braces are versatile and are okay anywhere, since we don't have a conditional expression to parse.
					 # The closing brace can also be kept on the same line since it is not preceded by an expression.
	let i += 1
}
set B = . A				 # Make a 'deep_copy' of A
set C = A				 # Make a shallow copy(?) of A (simple assignment)
under A[0] : 1 -> True			 # Add a mapping to the map in set A, at index 0 (testmap). Note that this mapping should 'not' be present in B now. 
print B, '\n'			         # So if B still shows an empty map at index 0, deep_copy successful!
print C, '\n'				 # But C 'should' have a non-empty map!
quit					 # That's all for now, folks!


# The sample output for this program will be (well, should be):
# ("Foo")
# {{}, ("Foo"), 3}
# {{(1, True)}, ("Foo"), 3}
