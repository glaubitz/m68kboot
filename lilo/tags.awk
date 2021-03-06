#
# tags.awk -- Convert tags.def files to C headers and tables
#
# $Id: tags.awk,v 1.2 1998-02-26 10:09:06 rnhodek Exp $
#
# $Log: tags.awk,v $
# Revision 1.2  1998-02-26 10:09:06  rnhodek
# New TAGTYPE_CARRAY; plain ARRAY didn't work correctly for string arrays.
#
# Revision 1.1  1997/08/12 15:26:58  rnhodek
# Import of Amiga and newly written Atari lilo sources, with many mods
# to separate out common parts.
#
#

BEGIN {
  OFS = ""; ORS = "";
  labelno = 0;
}

/^[ \t]*#/ || /^[ \t]*$/ { next }

$2 ~ /^section$/ {
  if (!($1 in seen)) {
	seen[$1] = 1;
	label[labelno++] = $1;
  }
  
  if (!($1 in struct_def))
	struct_def[$1] = struct_def[$1] "struct " $3 " {\n";
  if (!($1 in table)) {
	table[$1] = table[$1] "static TAGTAB " $3 "_table[] = {\n";
	anchor[$1] = $5;
	configanchor[$1] = $6;
	name[$1] = $7;
	for( i = 8; i <= NF; ++i )
	  name[$1] = name[$1] " " $i;
  }
  struct_name[$1] = $3;
  link_field[$1] = "-1";
  curr_tagno[$1] = $4;
  tagno_cnt[$1] = 0;

  if (in_section) {
	print "    END_TAGTAB\n};\n\n" >>c_file;
  }
  next;
}

{
  tag_name = $2; type = $3; size = $4; varname = $5;
  comment = "/* ";
  for( i = 6; i <= NF; ++i )
	comment = comment $i " ";
  comment = comment "*/";
	
  if (tag_name != "-") {
	defs[$1] = defs[$1] "#define TAG_" tag_name "\t";
	if (length(tag_name) <= 11)
	  defs[$1] = defs[$1] "\t";
	defs[$1] = defs[$1] "(" curr_tagno[$1]++ ")\t" comment "\n";
	tagnames = tagnames "    { TAG_" tag_name ", \"TAG_" tag_name "\" },\n";
	
	if (tagno_cnt[$1] == 0)
	  start_tag[$1] = tag_name;
	if (tagno_cnt[$1] == 1)
	  end_tag[$1] = tag_name;
	tagno_cnt[$1]++;
  }
  if (type != "-") {
	if (type ~ /^#/) {
      # special C type
	  ctype = "const " substr( type, 2 );
	  gsub( /-/, " ", ctype );
	  ttype = "INT"; # anything...
	}
	else if (type == "int") {
	  ctype = "const u_long *";
	  ttype = "INT";
	}
	else if (type == "str") {
	  ctype = "const char   *";
	  ttype = "STR";
	}
	else if (type == "link") {
	  ctype = "struct " struct_name[$1] " *";
	  ttype = "";
	  link_field[$1] = "offsetof(struct " struct_name[$1] "," varname ")";
	}
	else {
	  print FILENAME, ":", NR, ": Bad type ", type, "\n";
	  ctype = "BADTYPE ";
	  ttype = "";
	}
	struct_def[$1] = struct_def[$1] "    " ctype varname;
	if (size != "-")
	  struct_def[$1] = struct_def[$1] "[" size "]";
	struct_def[$1] = struct_def[$1] ";\n";

	if (ttype != "") {
	  if (size == "-")
		size = 0;
	  else {
		if (ttype == "INT")
		  ttype = "ARRAY";
		else if (ttype == "STR")
		  ttype = "CARRAY";
		else {
		  print FILENAME, ":", NR, ": Bad type ", type, "\n";
		  ttype = "BADTYPE";
		}
	  }
	  table[$1] = table[$1] "    { TAG_" tag_name ", TAGTYPE_" ttype \
							", offsetof(struct " struct_name[$1] "," \
							varname "), " size " },\n";
    }
  }
}

END {
  if (header != "") {
	print "/* automatically generated by tags.awk -- do not edit */\n" >header;
	print "\n#ifndef _tagdef_h\n#define _tagdef_h\n\n" >>header;
	for( i = 0; i < labelno; ++i ) {
	  lab = label[i];
	  print defs[lab] "\n" >>header;
	  print struct_def[lab] "};\n\n" >>header;
	}
	print "\n#endif /* _tagdef_h */\n" >>header;
  }

  if (c_file != "") {
	print "/* automatically generated by tags.awk -- do not edit */\n\n" >c_file;
	for( i = 0; i < labelno; ++i ) {
 	  lab = label[i];
	  print table[lab] "    END_TAGTAB\n};\n\n" >>c_file;
	}
	print "\n" >>c_file;
  }
  print "TAGSECT TagSections[] = {\n" >>c_file;
  for( i = 0; i < labelno; ++i ) {
	lab = label[i];
	print "    { \"" name[lab] "\",\n" \
	      "#ifdef FROM_WRITETAGS\n" \
	      "      " \
	      "(char **)&Config." configanchor[lab] ",\n" \
	      "#else /* FROM_WRITETAGS */\n" \
	      "      " \
	      "(char **)&" anchor[lab] ",\n" \
	      "#endif /* FROM_WRITETAGS */\n" \
	      "      " \
	      link_field[lab] ", " \
	      "\n      " \
	      "TAG_" start_tag[lab] ", " \
	      "TAG_" end_tag[lab] ", " \
	      "\n      " \
	      struct_name[lab] "_table, " \
	      "sizeof(struct " struct_name[lab] ") },\n" >>c_file;
  }
  print "};\n\n" >>c_file;

  if (tagnames_file != "") {
	print "/* automatically generated by tags.awk -- do not edit */\n\n" >tagnames_file;
	print "static struct TagName TagNames[] = {\n" >>tagnames_file;
	print tagnames >>tagnames_file;
	print "};\n\n" >>tagnames_file;
  }
}

