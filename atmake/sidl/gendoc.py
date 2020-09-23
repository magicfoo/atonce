

import os, sys
import atmake
import atmake.tools.fs as fs
from pygccxml import declarations
from .components import *
from .error import *
from xml.dom.minidom import Document, parseString


BALISE_DOC = "DOC"
BALISE_ITF = "ITF"
BALISE_IMP = "IMP"
BALISE_FUN = "FUN"
BALISE_ARG = "ARG"
BALISE_TYPE = "TYPE"
BALISE_NAMESPACE = "NAMESPACE"
BALISE_CLASS = "CLASS"
BALISE_METHOD = "METHOD"
BALISE_ARGUMENT = "argument"
BALISE_CONCERNS = "concerns"
BALISE_INTERFACE = "interface"
BALISE_IMPLEMENTATION = "implementation"
BALISE_FUNCTION = "function"
BALISE_CLASS_INSIDE = "class"
BALISE_NAMESPACE_INSIDE = "namespace"
BALISE_TEST = "TEST"
BALISE_TITLE = "title"
BALISE_VERSION = "version"
BALISE_DATE = "date"
BALISE_BRIEF = "brief"
BALISE_LONG = "long"
BALISE_GOTO = "goto"
BALISE_REF = "ref"
BALISE_AUTHOR = "author"

ATTRIBUT_RET = "return"
ATTRIBUT_ID = "id"
ATTRIBUT_NAME = "name"
ATTRIBUT_TYPE = "type"
ATTRIBUT_FILE = "file"
ATTRIBUT_LINE = "line"
ATTRIBUT_EXT = "extends"
ATTRIBUT_REF = "ref"
ATTRIBUT_BLOCK_IN = "blockin"
ATTRIBUT_BLOCK_OUT = "blockout"
ATTRIBUT_CONTEXT = "context"
ATTRIBUT_SOURCE = "source"
ATTRIBUT_SIG = "signature"

DOC_ITF = "itf"
DOC_IMP = "imp"
DOC_FUN = "fun"
DOC_TITLE = "title"
DOC_VERSION = "version"
DOC_DATE = "date"
DOC_BRIEF = "brief"
DOC_LONG = "long"
DOC_GOTO = "goto"
DOC_REF = "ref"
DOC_AUTHOR = "author"

BOOK_TITLE = "bookTitle"
BOOK_CHAPTER = "chapter"
BOOK_SECTION = "section"
BOOK_SUBSECTION = "subsection"
BOOK_INPUT = "input"
BOOK_ALL = "allitems"
BOOK_DOC = "doc"

SYMBOL_ITEM_AFTER = "$+"
SYMBOL_ITEM_BEFORE = "$-"
SYMBOL_ITEM_PARENT = "$$"

TEX_HEAD = b'\documentclass[a4paper,11pt]{report}\n\\usepackage[Bjornstrup]{fncychapmod}\n\\usepackage[latin1]{inputenc}\n\usepackage[T1]{fontenc}\n\\usepackage[normalem]{ulem}\n\\usepackage[french]{babel}\n\\usepackage{verbatim}\n\\usepackage{graphicx}\n\\begin{document}\n'
TEX_FOOT = b'\n\end{document}'
TEX_FILE = ''
TEX_PARAG = "\\paragraph{}"
TEX_DATE = "Date :"
TEX_VERSION = "Version :"
TEX_AUTHOR = "Author :"
TEX_NO_DOC = "Documentation is not available"
TEX_BEGIN_FUNCTIONS = "\\section{Functions of "
TEX_LINE = "\n\n"

TITLE = ""
TMP = ""

def SIDL_GenDoc ( sidl=None, cpp=None, verbose=False ) :

	if sidl is None :
		return
		
	outxmlfile = sidl.SubstModule( 'gendoc', 'xmlfile' )
	if not outxmlfile :
		return

	IDCount = 0;
	
	document = Document()
	IDObjects = {}
	IDStrings = {}
	IDNamespaces = {}
	doc = document.createElement(BALISE_DOC)
	interfacesInDoc = {}
	
		
	#------------------------------------------------------------------
	#Functions to associate ids to sidl objects
	
	def parse_itf(interface, id) :
		IDObjects[interface] = id
		IDStrings[interface.classname] = id
		for fun in interface.functions :
			id += 1
			IDObjects[fun] = id
		return id
		
	def parse_imp(implementation, id) :
		IDObjects[implementation] = id
		IDStrings[implementation.classname] = id
	
	def parse_test(test, id) :
		IDObjects[test] = id
		IDStrings[test.uri] = id
	
	
	#---------------------------------------------------------------------
	#Functions to build XML tree
	
	def export_itf_doc(doc, interface, id):
	
		itf = document.createElement(BALISE_ITF)
		
		itf.setAttribute(ATTRIBUT_ID, "%i"%IDObjects[interface])
		itf.setAttribute(ATTRIBUT_NAME, interface.classname)
		if not (interface.super is None):
			itf.setAttribute(ATTRIBUT_EXT, "%i"%IDObjects[interface.super])
		itf.setAttribute(ATTRIBUT_FILE, interface.loc.filename)
		itf.setAttribute(ATTRIBUT_LINE, "%i"%interface.loc.lineno)
		if not (interface.blockin == None):
			itf.setAttribute(ATTRIBUT_BLOCK_IN, "%i"%interface.blockin.lineno)
			itf.setAttribute(ATTRIBUT_BLOCK_OUT, "%i"%interface.blockout.lineno)
	
		doc.appendChild(itf)
		
		for fun in interface.functions :
			function = document.createElement(BALISE_FUNCTION);
			function.setAttribute(ATTRIBUT_TYPE, "%i"%IDObjects[fun])
			itf.appendChild(function)
			id = export_fun_doc(doc, fun, itf.getAttribute(ATTRIBUT_ID), id)
					
		return id
		
			
	def export_imp_doc(doc, implementation):
	
		imp = document.createElement(BALISE_IMP)
		
		imp.setAttribute(ATTRIBUT_ID, "%i"%IDObjects[implementation])
		imp.setAttribute(ATTRIBUT_NAME, implementation.classname)
		imp.setAttribute(ATTRIBUT_FILE, implementation.loc.filename)
		imp.setAttribute(ATTRIBUT_LINE, "%i"%implementation.loc.lineno)
		if not (implementation.blockin == None):
			imp.setAttribute(ATTRIBUT_BLOCK_IN, "%i"%implementation.blockin.lineno)
			imp.setAttribute(ATTRIBUT_BLOCK_OUT, "%i"%implementation.blockout.lineno)
		
		for obj in implementation.concerns :
			itf = document.createElement(BALISE_CONCERNS)
			itf.setAttribute(ATTRIBUT_TYPE, "%i"%IDObjects[obj])
			imp.appendChild(itf)
		
		doc.appendChild(imp)
		
		
	def export_fun_doc(doc, function, idInterface, id):
		fun = document.createElement(BALISE_FUN)
		fun.setAttribute(ATTRIBUT_ID, "%i"%IDObjects[function])
		fun.setAttribute(ATTRIBUT_NAME, function.name)
		fun.setAttribute(ATTRIBUT_CONTEXT, idInterface)
		fun.setAttribute(ATTRIBUT_FILE, function.loc.filename)
		fun.setAttribute(ATTRIBUT_LINE, "%i"%function.loc.lineno)
		if not (function.blockin == None):
			fun.setAttribute(ATTRIBUT_BLOCK_IN, "%i"%function.blockin.lineno)
			fun.setAttribute(ATTRIBUT_BLOCK_OUT, "%i"%function.blockout.lineno)
		
		fun.setAttribute(ATTRIBUT_RET, function.rtype)
		

		doc.appendChild(fun)

		for args in function.args :
			newArgument = document.createElement(BALISE_ARGUMENT)
			newArgument.setAttribute(ATTRIBUT_NAME, args[1])
			newArgument.setAttribute(ATTRIBUT_TYPE, args[0])

			fun.appendChild(newArgument)

		return id
		
		
	def export_type_doc(doc, type, id):
		result = IDStrings.get(type, None)
		if result == None :
			newType = document.createElement(BALISE_TYPE)
			id+=1
			newType.setAttribute(ATTRIBUT_ID, "%i"%id)
			newType.setAttribute(ATTRIBUT_NAME, type)
			doc.appendChild(newType)
			IDStrings[type] = id
			result = id
			
		return result, id
		
		
	def export_test_doc(doc, tunit) :
		test = document.createElement(BALISE_TEST);
		test.setAttribute(ATTRIBUT_ID, "%i"%IDObjects[tunit])
		test.setAttribute(ATTRIBUT_NAME, tunit.uri)
		test.setAttribute(ATTRIBUT_FILE, tunit.loc.filename)
		test.setAttribute(ATTRIBUT_LINE, "%i"%tunit.loc.lineno)
		test.setAttribute(ATTRIBUT_BLOCK_IN, "%i"%tunit.blockin.lineno)
		test.setAttribute(ATTRIBUT_BLOCK_OUT, "%i"%tunit.blockout.lineno)
		doc.appendChild(test)
		
	
	#---------------------------------------------------------
	#Functions to parse documentation blocs
	
	def parse_text_doc(docObj, xml, symbolsList):
		docText = docObj.GetText()
		docText = docText.replace('\n', ' ')
		list = docText.split('\\')
		
		elementEmpty = True
		currentItem = None
		currentBlock = BALISE_LONG
		
		currentItem = search_before_item(xml, docObj)
				
		if  list[0] == '  ' :
			list.pop(0)
		for i in range(len(list[0])) :
			if ord(list[0][i]) > 32 :
				elementEmpty = False
				break;
		
		if elementEmpty :
			list.pop(0)
		
		if list[0] == "" :
			list.pop(0)
		
		for text in list :
			position = search_first_placement(text)
			
			if position == -1 :
				typeItem = text
				text = ""
			else :
				typeItem = text[0:position]
				text = text[position+1:len(text)]
			
			text = replace_all_symbols_in_text(docObj, xml, text, symbolsList)
			if (typeItem == SYMBOL_ITEM_AFTER) :
				currentItem = docObj.env.GetVar(typeItem)
				if currentItem == None :
					currentItem = search_after_item(xml, docObj)
					docObj.env.SetVar(typeItem, currentItem)
				add_if_not_empty(currentItem, BALISE_LONG, text, docObj)
				currentBlock = BALISE_LONG
				
			elif typeItem == SYMBOL_ITEM_BEFORE :
				currentItem = docObj.env.GetVar(typeItem)
				if currentItem == None :
					currentItem = search_before_item(xml, docObj)
					docObj.env.SetVar(typeItem, currentItem)
				add_if_not_empty(currentItem, BALISE_LONG, text, docObj)
				currentBlock = BALISE_LONG	
				
			elif typeItem == SYMBOL_ITEM_PARENT :
				currentItem = docObj.env.GetVar(typeItem)
				if currentItem == None :
					currentItem = search_bounding_item(xml, docObj)
					docObj.env.SetVar(typeItem, currentItem)
				add_if_not_empty(currentItem, BALISE_LONG, text, docObj)
				currentBlock = BALISE_LONG	
				
			elif typeItem == DOC_ITF :
				currentItem = search_item(BALISE_ITF, text, xml, docObj)
				currentBlock = BALISE_LONG
				text=' '
				
			elif typeItem == DOC_IMP :
				currentItem = search_item(BALISE_IMP, text, xml, docObj)
				currentBlock = BALISE_LONG
				text=' '
					
			elif typeItem == DOC_FUN :
				currentItem = search_item(BALISE_FUN, text, xml, docObj)
				currentBlock = DOC_LONG
				text=' '
				
			elif currentItem == None :
				SIDL_Error( "Undefined item for this documentation : '%s', at position %i"%(text, position), docObj.loc ) 
				
			elif typeItem == DOC_TITLE :
				add_if_not_empty(currentItem, BALISE_TITLE, text, docObj)
				currentBlock = BALISE_TITLE
				
			elif typeItem == DOC_VERSION :
				add_if_not_empty(currentItem, BALISE_VERSION, text, docObj)
				currentBlock = BALISE_VERSION
				
			elif typeItem == DOC_DATE :
				add_if_not_empty(currentItem, BALISE_DATE, text, docObj)
				currentBlock = BALISE_DATE
				
			elif typeItem == DOC_BRIEF :
				add_if_not_empty(currentItem, BALISE_BRIEF, text, docObj)
				currentBlock = BALISE_BRIEF
				
			elif typeItem == DOC_AUTHOR :
				add_if_not_empty(currentItem, BALISE_AUTHOR, text, docObj)
				currentBlock = BALISE_AUTHOR
								
			elif typeItem[0:4] == DOC_GOTO :
				currentNode = currentItem.getElementsByTagName(currentBlock)[0]
				add_item_doc_ref(BALISE_GOTO, currentNode, typeItem, text, docObj)
			
			elif typeItem[0:3] == DOC_REF :
				currentNode = currentItem.getElementsByTagName(currentBlock)[0]
				add_item_doc_ref(BALISE_REF, currentNode, typeItem, text, docObj)
				
			else :
				currentBlock = BALISE_LONG
				add_if_not_empty(currentItem, BALISE_LONG, text, docObj)
			
	def search_first_placement(text) :
		for i in range(len(text)) :
			if ((ord(text[i])) <= 32) :
				return i
		return -1
	
	def add_if_not_empty(currentItem, currentBlock, text, docObj) :
		for i in range(len(text)) :
			if ((ord(text[i])) >32) :
				add_item_doc(currentItem, currentBlock, text, docObj)
				break;
	

	def search_item(balise, text, xml, docObj) :
		sig = None
		for i in range(len(text)) :
			if (ord(text[i]) <= 32) : 
				sig = text[0:i]
				break;
			
		newItem = None
		if (sig.find(':') == -1) :
			listXML = xml.getElementsByTagName(balise);
			for node in listXML :
								
				if node.getAttribute(ATTRIBUT_NAME) == sig :
					newItem = node
					break;
		else :
			#For moment, we consider only case where we deal with a function
		
			interface_name = sig[0:sig.find(':')]
			function_name = sig[sig.find(':')+2:len(sig)]
			listItfs = xml.getElementsByTagName(BALISE_ITF)
			listFuns = xml.getElementsByTagName(BALISE_FUN)
			for node in listItfs :
				if (node.getAttribute(ATTRIBUT_NAME) == interface_name) :
					itf_node = node
					listFunsInNode = node.getElementsByTagName(BALISE_FUNCTION)
					for funXML in listFunsInNode:
						id = funXML.getAttribute("type")
						for funNode in listFuns :
							if funNode.getAttribute("id") == id and funNode.getAttribute("name") == function_name :
								newItem = funNode
								break
		
		if newItem is None :
			SIDL_Error( 'Signature unidentified for balise %s with argument "%s"'%(balise, text), docObj.loc)

		return newItem
	

	def add_item_doc(item, string, text, docObj) :
		if item == None :
			SIDL_Error( "Item undefined", docObj.loc )
		if len(item.getElementsByTagName(string)) > 0 :
			SIDL_Error( "Balise %s already defined for %s"%(string,item.getAttribute("name")), docObj.loc ) 
		else :
			newNode = document.createElement(string)
			newNode.appendChild(document.createTextNode(text))
			item.appendChild(newNode)
	
	
	def add_item_doc_ref(balise, item, typeItem, text, docObj) :
		if (typeItem.find('}') != -1) :
			ref = typeItem[typeItem.find('{')+1:typeItem.find('}')]
		else :
			SIDL_Error( "Syntax error in signature : '%s', at position %i"%(text, position), docObj.loc )
		name = None
		if (typeItem.find(']') != -1) :
			if typeItem.find('[') == -1 :
				SIDL_Error( "Syntax error in link : '%s', at position %i"%(text, position), docObj.loc )
			name = typeItem[typeItem.find('[')+1:typeItem.find(']')]
			if not (typeItem.find(']') == len(typeItem)):
				text = ''.join([(typeItem.split(']')[1]), ' ', text])
		else :
			
			name = typeItem[typeItem.find('[')+1:len(typeItem)]
			listText = text.split(']')
			name = ''.join([name, ' ', listText[0]])
			text = listText[1]
			
		goto = document.createElement(balise)
		
		goto.setAttribute(ATTRIBUT_REF, str(IDStrings[ref]))
		nodeText = document.createTextNode(name)
		goto.appendChild(nodeText)
		item.appendChild(goto)
		item.appendChild(document.createTextNode(text))
	
	
	def search_bounding_item(xml, docObj) :
		closerItem = None
		diffLines = None
		
		closerItem, diffLines = search_closer_item_before(BALISE_ITF, closerItem, diffLines, xml, docObj)
		closerItem, diffLines = search_closer_item_before(BALISE_IMP, closerItem, diffLines, xml, docObj)
		
		if not (closerItem == None) :
			blockOut = closerItem.getAttribute(ATTRIBUT_BLOCK_OUT)
			if not (blockOut == None) :
				if (docObj.loc.lineno < int(blockOut)) :
					return closerItem
		return None
	
	
	def search_before_item(xml, docObj) :
		closerItem = None
		diffLines = None
		
		closerItem, diffLines = search_closer_item_before(BALISE_ITF, closerItem, diffLines, xml, docObj)
		closerItem, diffLines = search_closer_item_before(BALISE_IMP, closerItem, diffLines, xml, docObj)
		closerItem, diffLines = search_closer_item_before(BALISE_FUN, closerItem, diffLines, xml, docObj)
		
		return closerItem

	def search_after_item(xml, docObj) :
		closerItem = None
		diffLines = None
		
		closerItem, diffLines = search_closer_item_after(BALISE_ITF, closerItem, diffLines, xml, docObj)
		closerItem, diffLines = search_closer_item_after(BALISE_IMP, closerItem, diffLines, xml, docObj)
		closerItem, diffLines = search_closer_item_after(BALISE_FUN, closerItem, diffLines, xml, docObj)
		
		return closerItem
		
		
	def search_closer_item_before(balise, item, diffLines, xml, docObj) :
		listXML = xml.getElementsByTagName(balise)
		for nodeXML in listXML :
			if (nodeXML.getAttribute("file") == docObj.loc.filename) :
				line1 = int(nodeXML.getAttribute(ATTRIBUT_LINE))
				line2 = docObj.loc.lineno
				if ((diffLines is None) or (line2-line1 < diffLines)) and (0 < line2-line1) :
					diffLines = line2-line1
					item = nodeXML
		return item, diffLines
	
	
	def search_closer_item_after(balise, item, diffLines, xml, docObj) :
		listXML = xml.getElementsByTagName(balise)
		for nodeXML in listXML :
			if (nodeXML.getAttribute("file") == docObj.loc.filename) :
				line1 = int(nodeXML.getAttribute(ATTRIBUT_LINE))
				line2 = docObj.loc.lineno
				if ((diffLines is None) or (abs(line2-line1) < abs(diffLines))) and (line2-line1 < 0) :
					diffLines = line2-line1
					item = nodeXML
		return item, diffLines
		
	
	# ----------------------------------------------------------------
	# Functions dealing with substitution symbols
		
	def replace_all_symbols_in_text(docObj, xml, text, symbolsList) :
		result = text
		listTmp = list(symbolsList)
		while not listTmp == [] :
			if (0 <= text.find(listTmp[0])) :
				itemTmp = None
				symbol = listTmp[0]
				if (docObj.env.GetVar(symbol) == None) :
					if (symbol == SYMBOL_ITEM_AFTER) :
						itemTmp = search_after_item(xml, docObj)
					if (symbol == SYMBOL_ITEM_BEFORE) :
						itemTmp = search_before_item(xml, docObj)
					if (symbol == SYMBOL_ITEM_PARENT) :
						itemTmp = search_bounding_item(xml, docObj)
					
					if itemTmp == None :
						SIDL_Error( "Cannot substitute %s for : '%s'"%(symbol, text), docObj.loc ) 
					docObj.env.SetVar(symbol, itemTmp)
					
				result = result.replace(listTmp[0], (docObj.env.GetVar(symbol)).getAttribute(ATTRIBUT_NAME))
			
			listTmp.pop(0)
		return result
	
	
	def empty_symbols_association(env, symbolsList) :
		for symbol in symbolsList :
			env.SetVar(symbol, None)

			
	#------------------------------------------------------------
	# Checking of PyGCCXML structure
		
	def extract_new_namespaces(list, id) :
		listNamespace = []
		listTmp = []
		for decl in list :
			if isinstance(decl, declarations.namespace.namespace_t) :
				id += 1
				listNamespace.extend([decl])
				IDObjects[decl] = id
				listTmp, id = extract_new_namespaces(decl.declarations, id)
				listNamespace.extend(listTmp)
				
		return listNamespace, id
	
	
	def extract_new_classes(listNm, id) :
		listClasses = []
		for itemNamespace in listNm :
			list = itemNamespace.declarations
			for item in list :
				if isinstance(item, declarations.class_declaration.class_t) :
					notFound = True
					for sidlItf in doc.getElementsByTagName(BALISE_ITF) :
						if (item.name == sidlItf.getAttribute(ATTRIBUT_NAME)) :
							if IDNamespaces.get(itemNamespace) == None :
								IDNamespaces[itemNamespace] = []
							IDNamespaces[itemNamespace].extend([sidlItf.getAttribute(ATTRIBUT_ID)])
							notFound = False
							break
					if notFound :
						listClasses.extend([item])
						id += 1
						IDObjects[item] = id
						
		return listClasses, id
			
			
	def extract_new_functions(listClasses, id) :
		listFun = []
		for item in listClasses :
			listMembers = item.get_members()
			for itemMember in listMembers :
				if isinstance(itemMember, declarations.calldef.member_function_t) :
					listFun.extend([itemMember])
					id += 1
					IDObjects[itemMember] = id
					
		return listFun, id
		
	
	def add_new_namespaces_in_xml(sidlDoc, listNamespaces) :
		for namespace in listNamespaces :
			nm = document.createElement(BALISE_NAMESPACE)
			nm.setAttribute(ATTRIBUT_ID, "%i"%IDObjects[namespace])
			nm.setAttribute(ATTRIBUT_NAME, namespace.name)
			if namespace.location != None :
				nm.setAttribute(ATTRIBUT_FILE, namespace.location.file_name)
				nm.setAttribute(ATTRIBUT_LINE, "%i"%namespace.location.line)
			
			if IDNamespaces.get(namespace) != None :
				for itf in IDNamespaces[namespace] :
					newItf = document.createElement(BALISE_CLASS_INSIDE)
					newItf.setAttribute(ATTRIBUT_TYPE, itf)
					nm.appendChild(newItf)
			
			for itemMember in namespace.declarations :
				if isinstance(itemMember, declarations.class_declaration.class_t) and IDObjects.get(itemMember) != None :
					newClass = document.createElement(BALISE_CLASS_INSIDE)
					newClass.setAttribute(ATTRIBUT_TYPE, "%i"%IDObjects[itemMember])
					nm.appendChild(newClass)
					
				if isinstance(itemMember, declarations.namespace.namespace_t) and IDObjects.get(itemMember) != None :
					newNM = document.createElement(BALISE_NAMESPACE_INSIDE)
					newNM .setAttribute(ATTRIBUT_TYPE, "%i"%IDObjects[itemMember])
					nm.appendChild(newNM)
			
			sidlDoc.appendChild(nm)

		
	def add_new_classes_in_xml(sidlDoc, listClasses) :
		for item in listClasses :
			cl = document.createElement(BALISE_CLASS)
			cl.setAttribute(ATTRIBUT_ID, "%i"%IDObjects[item])
			cl.setAttribute(ATTRIBUT_NAME, item.name)
			cl.setAttribute(ATTRIBUT_CONTEXT, "%i"%IDObjects[item.parent])
			if item.location != None :
				cl.setAttribute(ATTRIBUT_FILE, item.location.file_name)
				cl.setAttribute(ATTRIBUT_LINE, "%i"%item.location.line)
			
			for itemMember in item.get_members() :
				if isinstance(itemMember, declarations.calldef.member_function_t) :
					method = document.createElement(BALISE_FUNCTION)
					method.setAttribute(ATTRIBUT_TYPE, "%i"%IDObjects[itemMember])
					cl.appendChild(method)
			
			sidlDoc.appendChild(cl)
			

	def add_new_functions_in_xml(sidlDoc, listFunctions) :
		for item in listFunctions :
			fun = document.createElement(BALISE_METHOD)
			fun.setAttribute(ATTRIBUT_ID, "%i"%IDObjects[item])
			fun.setAttribute(ATTRIBUT_NAME, item.name)
			fun.setAttribute(ATTRIBUT_CONTEXT, "%i"%IDObjects[item.parent])
			fun.setAttribute(ATTRIBUT_RET, str(item.return_type))
			if item.location != None :
				fun.setAttribute(ATTRIBUT_FILE, item.location.file_name)
				fun.setAttribute(ATTRIBUT_LINE, "%i"%item.location.line)
			for arg in item.arguments :
				newArg = document.createElement(BALISE_ARGUMENT)
				newArg.setAttribute(ATTRIBUT_NAME, arg.name)
				newArg.setAttribute(ATTRIBUT_TYPE, str(arg.type))
				fun.appendChild(newArg)
			
			sidlDoc.appendChild(fun)

	
	#--------------------------------------------------------
	# Listing substitution symbols
	
	symbolsList = [ SYMBOL_ITEM_AFTER, SYMBOL_ITEM_BEFORE, SYMBOL_ITEM_PARENT ]
		
	
	#---------------------------------------------------------
	# Associations of IDs to sidl objects
	
	for obj in sidl.interfaces.values() :
		IDCount += 1
		IDCount = parse_itf(obj, IDCount)
		
	for obj in sidl.implementations.values() :
		IDCount += 1
		parse_imp(obj, IDCount)

	for obj in sidl.tests.values() :
		IDCount += 1
		parse_test(obj, IDCount)
		
		
	#------------------------------------------------------------
	# Building of XML tree
	
	for obj in sidl.interfaces.values() :
		IDCount = export_itf_doc(doc, obj, IDCount)
		
	for obj in sidl.implementations.values():
		export_imp_doc(doc, obj)
	
	for obj in sidl.tests.values() :
		export_test_doc(doc, obj)
	
	
	#--------------------------------------------------------------
	# Parsing of documentation blocs
	
	for docText in sidl.docs :
		parse_text_doc(docText, doc, symbolsList)
		empty_symbols_association(docText.env, symbolsList)
	
	
	#---------------------------------------------------------------
	# Merging with gccxml result

	listDecl = cpp[0].declarations
	listNamespaces, IDCount = extract_new_namespaces(listDecl, IDCount)
	listClasses, IDCount = extract_new_classes(listNamespaces, IDCount)
	listFunctions, IDCount = extract_new_functions(listClasses, IDCount)
	
	add_new_namespaces_in_xml(doc, listNamespaces)
	add_new_classes_in_xml(doc, listClasses)
	add_new_functions_in_xml(doc, listFunctions)
	
	
	#---------------------------------------------------------------
	# Writing of latex file
	

	def adapt_text(text) :
		text = text.replace("<", "$<$")
		text = text.replace(">", "$>$")
		text = text.replace("_", "\_")
		return text
	
	def latex_doc(sidlDoc) :
		list = sidlDoc.childNodes
		text = [TEX_HEAD]
		for item in list :
			text.append("\chapter{")
			name = item.getAttribute(ATTRIBUT_NAME)
			name = adapt_text(name)
			text.append(name)
			text.append("}\n")
			listOfChildren = item.childNodes
			for child in listOfChildren :
				if (child.nodeName == BALISE_REF or child.nodeName == BALISE_LONG or child.nodeName == BALISE_DATE 
					or child.nodeName == BALISE_VERSION or child.nodeName == BALISE_AUTHOR) :
					text.append("\section{" + child.nodeName + "}\n")
				if (child.nodeType == child.TEXT_NODE) :
					text.append(child)
				listOfLittleChildren = child.childNodes
				for littleChild in listOfLittleChildren :
					if (littleChild.nodeType == littleChild.TEXT_NODE) :
						text.append(adapt_text(littleChild.data))
				
		text.append("\\end{document}")
		return text
	
	
	#print TITLE
	TEX_FILE = ''.join(latex_doc(doc))

	
	#---------------------------------------------------------------
	# Functions to build books
	
	def build_book_file(book, doc) :
	
		fileName = ""
		pathFile = ""
		finalText = ""
		titleDefined = False
		authorDefined = False
		pathDefined = False
		fileNameDefined = False
	
		bookText = book.GetText()
		list = bookText.split("\\body")
		
		if len(list) != 2 :
			SIDL_Error( "Book must have a body section !", book.loc )
		
		header = list[0]
		listHeader = header.split('\\')
		body = list[1]
		listBody = body.split('\\')
		listHeader.pop(0)
		for text in listHeader :
			position = find_end_position(text)
			typeItem = text[0:position]
			text = text[position+1:]
			
			if (typeItem[0:4] == "path") :
				if pathDefined :
					SIDL_Error( "Path already defined", book.loc )		
				
				pathFile = balise_content(typeItem, text, "path", book)
				pathFile = pathFile[pathFile.find('{')+1:pathFile.find('}')]
				pathDefined = True
			
			elif (typeItem[0:8] == "filename") :
				if fileNameDefined :
					SIDL_Error( "Filename already defined", book.loc )		
				
				fileName = balise_content(typeItem, text, "filename", book)
				fileName = fileName[fileName.find('{')+1:fileName.find('}')]
				fileNameDefined = True
			
			elif (typeItem[0:5] == "title") :
				if titleDefined :
					SIDL_Error( "Title already defined", book.loc )		
				
				textTitle = balise_content(typeItem, text, "title", book)
				finalText = ''.join([finalText, textTitle])
				titleDefined = True
				
			elif (typeItem[0:6] == "author") :
				if authorDefined :
					SIDL_Error( "Author already defined", book.loc )
				
				textTitle = balise_content(typeItem, text, "author", book)
				finalText = ''.join([finalText, textTitle])
				authorDefined = True
			elif (typeItem[0:10] == "usepackage") :
				finalText = ''.join([finalText,"\\", typeItem])
			else :
				SIDL_Error( "Balise %s is undefined"%(typeItem+text), book.loc )
		
		finalText = ''.join([finalText, "\\maketitle\n\\tableofcontents\n"])
		listBody.pop(0)
		for text in listBody :
			position = find_end_position(text)
			typeItem = text[0:position]
			balisePosition = typeItem.find('{')
			if (balisePosition == -1) :
				balise = typeItem
			else :
				balise = typeItem[:typeItem.find('{')]
			text = text[position+1:]
			if balise == BOOK_CHAPTER :
				finalText = ''.join([finalText, balise_content(typeItem, text, balise, book)])
				
			elif balise == BOOK_SECTION :
				finalText = ''.join([finalText, balise_content(typeItem, text, balise, book)])
				
			elif balise == BOOK_SUBSECTION :
				finalText = ''.join([finalText, balise_content(typeItem, text, balise, book)])
			
			elif balise == BOOK_INPUT :
				finalText = ''.join([finalText, "\\", typeItem, "\n"])
				
			elif balise == BOOK_DOC :
			
				finalText = ''.join([finalText, create_doc_of_element(typeItem, doc, book.loc)])
				finalText = ''.join([finalText, text])
				
			elif balise == BOOK_ALL :
				completeList = doc.getElementsByTagName(BALISE_ITF)
				completeList = completeList + doc.getElementsByTagName(BALISE_IMP)
				for interfaceXml in completeList :
					finalText = ''.join([finalText, create_doc_of_interface(interfaceXml, doc.childNodes)])
				
			else :
				SIDL_Error( "Balise %s is undefined"%(typeItem), book.loc )
	
		return pathFile, fileName, finalText
		
		
	def normalize_name(name) :
		if name.find('_') != -1:
			list = name.split('_')
			result = ""
			if (len(list) > 1) :
				result += list[0]
				list.pop(0)
				for elem in list :
					result += "\_" + elem
		else :
			return name
		
		return result
	
	def create_doc_of_interface(xmlNode, childList) :
		textResult = ""
		childNode = xmlNode.childNodes
		listOfFunction = xmlNode.getElementsByTagName(BALISE_FUNCTION)
		
		textResult = ''.join([textResult, extract_doc_book(xmlNode, "section")])
		if len(listOfFunction) != 0:
			textResult = ''.join([textResult, TEX_BEGIN_FUNCTIONS, normalize_name(xmlNode.getAttribute(ATTRIBUT_NAME)), "}" + TEX_LINE])
		
		for function in listOfFunction :
			idFunction = int(function.getAttribute(ATTRIBUT_TYPE))
			nodeFun = childList[idFunction-1]
			
			textResult = ''.join([textResult, extract_doc_book(nodeFun, "subsection")])
			
		return textResult
		
	def create_doc_of_element(typeItem, doc, loc) :
		textResult = ""
		positionStart = typeItem.find("{")
		positionEnd = typeItem.find("}")
		
		if ((positionStart == -1) or (positionEnd == -1)) :
			SIDL_Error( "Error in name %s"%(typeItem), book.loc )

		nameItem = typeItem[positionStart+1:positionEnd]

		id = IDStrings.get(nameItem)
		if (id == None) :
			SIDL_Error( "Error in name %s"%(typeItem), book.loc )
		childList = doc.childNodes
		
		node = childList[id-1]
		
		textResult = ''.join([textResult, create_doc_of_interface(node, childList)])
			
		return textResult

	def extract_doc_book(node, balise) :
		listOfChildren = node.childNodes
		author = ""
		date = ""
		version = ""
		brief = ""
		long = ""
		asDocumentation = False
		
		header = "\\" + balise + "{" + normalize_name(node.getAttribute(ATTRIBUT_NAME)) + "}\n"
		
		for child in listOfChildren :
			if child.nodeName == BALISE_AUTHOR :
				author = ''.join([TEX_AUTHOR + " ", extract_text(child), TEX_LINE])
				asDocumentation = True
			
			if child.nodeName == BALISE_VERSION :
				version = ''.join([TEX_VERSION + " ", extract_text(child), TEX_LINE])
				asDocumentation = True
			
			if child.nodeName == BALISE_DATE :
				date = ''.join([TEX_DATE + " ", extract_text(child), TEX_LINE])
				asDocumentation = True
				
			if child.nodeName == BALISE_BRIEF :
				brief = ''.join([TEX_PARAG, " ", extract_text(child), TEX_LINE])				
				asDocumentation = True
				
			if child.nodeName == BALISE_LONG :
				long = ''.join([TEX_PARAG, " ", extract_text(child), TEX_LINE])
				asDocumentation = True
		if (not asDocumentation) :
			return header + TEX_NO_DOC + TEX_LINE
				
		return header + author + date + version + brief + int
				
	def extract_text(node) :
		result = ""
		for littleChild in node.childNodes :
			nodeText = littleChild
			if littleChild.nodeName == BALISE_GOTO :
				for gotoChild in littleChild.childNodes :
					if gotoChild.nodeType == gotoChild.TEXT_NODE :
						result = ''.join([result, gotoChild.data])
			
			if littleChild.nodeName == BALISE_REF :
				for refChild in littleChild.childNodes :
					if refChild.nodeType == refChild.TEXT_NODE :
						result = ''.join([result, refChild.data])
			
			if nodeText.nodeType == nodeText.TEXT_NODE :
				result = ''.join([result, nodeText.data])
		return result
	
	def find_end_position(text) :
		for i in range(len(text)) :
			if ((ord(text[i])) < 32) :
				return i
		return len(text)

	def balise_content(typeItem, text, balise, book) :
		textTitle = ""
		positionStart = typeItem.find('{')
		if positionStart == -1 :
			SIDL_Error("Bad definition of balise %s, '{' not found"%balise, book.loc)
		positionEnd = typeItem.find('}')
		if positionEnd == -1 :
			textTitle = typeItem[positionStart+1:]
			positionEnd = text.find('}')
			if positionEnd == -1 :
				SIDL_Error( "Bad definition of balise %s, '}' not found"%balise, book.loc )
			textTitle = ''.join([" ", text[0:]])
		else :
			textTitle = typeItem[positionStart+1:] + text

		return "\\" + balise + "{" + textTitle + "\n"
	
	
	#---------------------------------------------------------------
	# Building of books
	
	texFiles = []
	bookCount = 1
	
	for book in sidl.books :
		pathFile, nameFile, text = build_book_file(book, doc)
		if pathFile == "" :
			pathTab = outxmlfile.split('/')
			pathFile = outxmlfile.replace(pathTab[len(pathTab)-1], "books/")
			
		if nameFile == "" :
			nameFile = "Book" + str(bookCount)
		texFiles.append([pathFile, nameFile, text])
		bookCount += 1
	
	
	#---------------------------------------------------------------
	#Writing of units tests
	
	def unicity_of_id(sidlDoc) :
		list = sidlDoc.childNodes
		tableCheck = {}
		i = 0
		for item in list :
			i += 1
			id = int(item.getAttribute(ATTRIBUT_ID))
			if tableCheck.get(id) == None :
				tableCheck[id] = i
			else :
				return False
		return True
		
	def id_ordered(sidlDoc) :
		list = sidlDoc.childNodes
		i = 1
		for item in list :
			if i != int(item.getAttribute(ATTRIBUT_ID)) :
				return False
			i += 1
		return True
		
	def unicity_of_class(sidlDoc) :
		list = sidlDoc.getElementsByTagName(BALISE_ITF)
		list.extend(sidlDoc.getElementsByTagName(BALISE_CLASS))
		
		for item1 in list :
			for item2 in list :
				if item1 != item2 :
					if ((item1.getAttribute(ATTRIBUT_NAME) == item2.getAttribute(ATTRIBUT_NAME)) 
						and (item1.getAttribute(ATTRIBUT_CONTEXT) == item2.getAttribute(ATTRIBUT_CONTEXT))) :
							#print item1.getAttribute(ATTRIBUT_NAME)
							return False
		return True
		
		
	def unicity_of_functions(sidlDoc) :
		list = sidlDoc.getElementsByTagName(BALISE_METHOD)
		list.extend(sidlDoc.getElementsByTagName(BALISE_FUN))
		
		for item1 in list :
			for item2 in list :
				if item1 != item2 :
					if ((item1.getAttribute(ATTRIBUT_NAME) == item2.getAttribute(ATTRIBUT_NAME)) 
						and (item1.getAttribute(ATTRIBUT_CONTEXT) == item2.getAttribute(ATTRIBUT_CONTEXT))
						and (item1.getAttribute(ATTRIBUT_RET) == item2.getAttribute(ATTRIBUT_RET))) :
							args1 = item1.getElementsByTagName(BALISE_ARGUMENT)
							args2 = item2.getElementsByTagName(BALISE_ARGUMENT)
							if len(args1) == len(args2) :
								sameArgs = True
								for i in range(len(args1)) :
									if args1[i].getAttribute(ATTRIBUT_TYPE) != args2[i].getAttribute(ATTRIBUT_TYPE) :
										sameArgs = False
								if sameArgs :
									#print item1.getAttribute(ATTRIBUT_NAME), item1.getAttribute(ATTRIBUT_CONTEXT)
									return False
		return True
		
	def test_tex_generator() :
		#aborted
		docTest = Document();
		xmlTest = docTest.createElement(BALISE_DOC)
		interTest = docTest.createElement(BALISE_ITF)
		interTest.setAttribute(ATTRIBUT_NAME, "interface 1")
		interTest.setAttribute(ATTRIBUT_ID, "1")
		briefTest = docTest.createElement(BALISE_BRIEF)
		briefTest.appendChild(docTest.createTextNode("brief"))
		interTest.appendChild(briefTest)
		xmlTest.appendChild(interTest)
		docTest.appendChild(xmlTest)
		#print docTest.toprettyxml()
	
	#test_tex_generator()
	
	def unit_tests(sidlDoc) :
		print("Unicity of IDs: ", unicity_of_id(sidlDoc))
		print("Order of IDs : ", id_ordered(sidlDoc))
		print("Unicity of class : ", unicity_of_class(sidlDoc))
		print("unicity of functions : ", unicity_of_functions(sidlDoc))
		
	
	#--------------------------------------------------------------
	#End of XML file building
	
		
	for texFile in texFiles :
		nameFile = texFile[0] + texFile[1] + ".tex"
		bodyFile = TEX_HEAD + texFile[2] + TEX_FOOT
		sidl.WriteLines( nameFile,  bodyFile )
	
	document.appendChild(doc)

	sidl.WriteLines( outxmlfile, document.toxml() )
	
	# Units tests
	#unit_tests(doc)
		
	IDObjects.clear()
	IDStrings.clear()
	IDNamespaces.clear()


