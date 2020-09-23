""" ATMake

Framework base class

"""


import os, sys, os.path, re
from atmake import *

__has_registry = False
try:
	import winreg
	__has_registry = True
	hkey_mod = winreg
	RegOpenKeyEx = winreg.OpenKeyEx
	RegOpenKey = winreg.OpenKey
	RegCreateKey = winreg.CreateKey
	RegDeleteKey = winreg.DeleteKey
	RegCloseKey = winreg.CloseKey
	RegEnumKey = winreg.EnumKey
	RegEnumValue = winreg.EnumValue
	RegQueryValueEx = winreg.QueryValueEx
	RegQueryInfoKey = winreg.QueryInfoKey
	RegSetValueEx = winreg.SetValueEx
	RegDeleteValue = winreg.DeleteValue
	RegError = winreg.error
	HKEY_CLASSES_ROOT = hkey_mod.HKEY_CLASSES_ROOT
	HKEY_LOCAL_MACHINE = hkey_mod.HKEY_LOCAL_MACHINE
	HKEY_CURRENT_USER = hkey_mod.HKEY_CURRENT_USER
	HKEY_USERS = hkey_mod.HKEY_USERS
except ImportError:
        class _NoError(Exception):
            pass
        RegError = _NoError


def HasRegistry ( ) :
	return __has_registry


def RegGetValue ( root, key ) :
	"""
	Instead of:
	k = RegOpenKeyEx( HKEY_LOCAL_MACHINE, r'SOFTWARE\Microsoft\Windows\CurrentVersion')
	out = RegQueryValueEx(k,'ProgramFilesDir')
	You can write:
	out = RegGetValue( HKEY_LOCAL_MACHINE, r'SOFTWARE\Microsoft\Windows\CurrentVersion\ProgramFilesDir' )
	"""
	if __has_registry :
		p = key.rfind('\\') + 1
		keyp = key[:p]
		val = key[p:]
		k = RegOpenKeyEx( root, keyp )
		return RegQueryValueEx( k, val )
	else :
		return None


def IsRegKey ( root, key ) :
	if __has_registry :
		try :
			k = RegOpenKeyEx( root, key )
			return True
		except RegError :
			return False
	else :
		return None


def IsRegValue ( root, key ) :
	if __has_registry :
		try :
			v = RegGetValue( root, key )
			return True
		except RegError :
			return False
	else :
		return None


def RegGlobKeys ( root, key, pattern ) :
	if __has_registry :
		if pattern :	_reobj = re.compile( pattern )
		else :	_reobj = None
		ks = []
		try :
			k = RegOpenKeyEx( root, key )
			ki = 0
			while True :
				kp = RegEnumKey( k, ki )
				if not _reobj or _reobj.match( kp ) :
					ks.append( kp )
				ki += 1
		except RegError :
			pass
		return ks
	else :
		return None


def RegListKeys ( root, key ) :
	return RegGlobKeys( root, key, None )


def RegGlobValues ( root, key, pattern ) :
	if __has_registry :
		if pattern :	_reobj = re.compile( pattern )
		else :	_reobj = None
		ks = []
		try :
			k = RegOpenKeyEx( root, key )
			ki = 0
			while True :
				kv = RegEnumValue( k, ki )
				if not _reobj or _reobj.match( kv[0] ) :
					ks.append( kv )
				ki += 1
		except RegError :
			pass
		return ks
	else :
		return None


def RegListValues ( root, key ) :
	return RegGlobValues( root, key, None )


def RegListShellFolders ( root, key ) :
	return RegListValues( HKEY_CURRENT_USER, r'Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders' )


def RegGetShellFolder ( dname ) :
	return RegGetValue( HKEY_CURRENT_USER, r'Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\%s' % dname )


def RegGetProgramFilesDir ( ) :
	return RegGetValue( HKEY_LOCAL_MACHINE, r'Software\\Microsoft\\Windows\\CurrentVersion\ProgramFilesDir' )


def RegGetSystemRoot ( ) :
	try :		return RegGetValue( HKEY_LOCAL_MACHINE, r'Software\Microsoft\Windows NT\CurrentVersion\SystemRoot' )
	except :	return r'C:\WINDOWS'


def RegGetNbProcessor ( ) :
	return os.environ['NUMBER_OF_PROCESSORS']
	#return RegGetValue( HKEY_LOCAL_MACHINE, r'Software\Microsoft\Windows NT\CurrentVersion\SystemRoot' )




def RegInstallKeys ( root, rkeys ) :
	""" register keys of the form :
		{ 'key' : [ (name,value) ] }
		if name is empty => this is the default value
	"""

	def install_value ( hkey, vname, vcontent ) :
		vtype = winreg.REG_SZ if atmake.IsString(vcontent) else winreg.REG_DWORD
		try :
			# already has content ?
			kv, kt = RegQueryValueEx( hkey, vname )
			assert( kt==vtype )
			if kt==winreg.REG_SZ :
				if not vcontent in kv :
					RegSetValueEx( hkey, vname, 0, vtype, '%s%s'%(kv,vcontent) )
			else :
				RegSetValueEx( hkey, vname, 0, vtype, vcontent )
		except :
			RegSetValueEx( hkey, vname, 0, vtype, vcontent )

	def install_key ( key, values ) :
		try:
			h = RegCreateKey( root, key )
			for (name,content) in values :
				install_value( h, name, content )
		except :
			pass

	for key,values in rkeys.items() :
		nkey = key.replace('/','\\').replace('\\\\','\\')
		install_key( nkey, values )




def RegUninstallKeys ( root,  rkeys ) :
	""" register keys of the form :
		{ 'key' : [ (name,value) ] }
		if name is empty => this is the default value
	"""

	def uninstall_value ( hkey, vname, vcontent ) :
		try:
			kv, kt = RegQueryValueEx( hkey, vname )
			if kt == winreg.REG_SZ :
				new_kv = kv.replace(vcontent,'').strip()
				if len(new_kv) :
					# not empty string value !
					RegSetValueEx( hkey, vname, 0, winreg.REG_SZ, new_kv )
					return
			# otherwize, delete the empty or numerical value 
			RegDeleteValue( hkey, vname )
		except :
			pass

	def uninstall_key ( key, values ) :
		try:
			h = RegOpenKey( root, key, 0, winreg.KEY_ALL_ACCESS )
			for (name,content) in values :
				uninstall_value( h, name, content )
			# remove key if empty
			nbsub, nbvalues, _ = RegQueryInfoKey( h )
			if nbsub==0 and nbvalues==0 :
				RegDeleteKey( root, key )
		except :
			pass

	for key,values in rkeys.items() :
		nkey = key.replace('/','\\').replace('\\\\','\\')
		uninstall_key( nkey, values )



