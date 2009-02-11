#!/usr/bin/env ruby

# Executes the svn status command and prints the entries
# into ChangeLog-like format for easy editing.
#
# Use in vim with !r svn2changelog.rb to paste a nice summary
# into ChangeLog.

# Customize here
name="Michael Dominic K."
email="michaldominik@gmail.com"

ary = []
`svn st`.each do |line|
        if /^\?.*/ =~ line or /^M\s*ChangeLog/ =~ line
        else
                ary << line.chomp.gsub!(/^.\s*/ , "")
        end
end

# Sort the entries
ary = ary.sort

# Print the header
print Time.now.strftime("%Y-%m-%d") + "  " + name + "  " + "<" + email + ">" " \n\n"

# Print all the entries
ary.each do |line|
        print "\t* " + line + ":\n"
end

print "\n"
