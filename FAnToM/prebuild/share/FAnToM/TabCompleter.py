#!/usr/bin/python
# -*- coding: UTF-8 -*-

# PSF LICENSE AGREEMENT FOR PYTHON 2.7.6

# This LICENSE AGREEMENT is between the Python Software Foundation (“PSF”), and the Individual or Organization (“Licensee”) accessing and otherwise using Python 2.7.6 software in source or binary form and its associated documentation.
# Subject to the terms and conditions of this License Agreement, PSF hereby grants Licensee a nonexclusive, royalty-free, world-wide license to reproduce, analyze, test, perform and/or display publicly, prepare derivative works, distribute, and otherwise use Python 2.7.6 alone or in any derivative version, provided, however, that PSF’s License Agreement and PSF’s notice of copyright, i.e., “Copyright © 2001-2014 Python Software Foundation; All Rights Reserved” are retained in Python 2.7.6 alone or in any derivative version prepared by Licensee.
# In the event Licensee prepares a derivative work that is based on or incorporates Python 2.7.6 or any part thereof, and wants to make the derivative work available to others as provided herein, then Licensee hereby agrees to include in any such work a brief summary of the changes made to Python 2.7.6.
# PSF is making Python 2.7.6 available to Licensee on an “AS IS” basis. PSF MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED. BY WAY OF EXAMPLE, BUT NOT LIMITATION, PSF MAKES NO AND DISCLAIMS ANY REPRESENTATION OR WARRANTY OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF PYTHON 2.7.6 WILL NOT INFRINGE ANY THIRD PARTY RIGHTS.
# PSF SHALL NOT BE LIABLE TO LICENSEE OR ANY OTHER USERS OF PYTHON 2.7.6 FOR ANY INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES OR LOSS AS A RESULT OF MODIFYING, DISTRIBUTING, OR OTHERWISE USING PYTHON 2.7.6, OR ANY DERIVATIVE THEREOF, EVEN IF ADVISED OF THE POSSIBILITY THEREOF.
# This License Agreement will automatically terminate upon a material breach of its terms and conditions.
# Nothing in this License Agreement shall be deemed to create any relationship of agency, partnership, or joint venture between PSF and Licensee. This License Agreement does not grant permission to use PSF trademarks or trade name in a trademark sense to endorse or promote products or services of Licensee, or any third party.
# By copying, installing or otherwise using Python 2.7.6, Licensee agrees to be bound by the terms and conditions of this License Agreement.

"""Word completion for command lines

This is taken from CPython rlcompleter and adapted for our purposes
to facilitate extended tab completion for Algorithm and Kernel objects
(like option names or algorithm names).

The completer completes keywords, built-ins and globals in a selectable
namespace (which defaults to __main__); when completing NAME.NAME..., it
evaluates (!) the expression up to the last dot and completes its attributes.

Notes:

- Exceptions raised by the completer function are *ignored* (and generally cause
  the completion to fail).  This is a feature -- we just complete as best as we can
  and don't want any verbose debug output such as backtraces.

- The evaluation of the NAME.NAME... form may cause arbitrary application
  defined code to be executed if an object with a __getattr__ hook is found.
  Since it is the responsibility of the application (or the user) to enable this
  feature, I consider this an acceptable risk.  More complicated expressions
  (e.g. function calls or indexing operations) are *not* evaluated.

"""

import __builtin__
import __main__

__all__ = ["Completer"]

class Completer:
    def __init__(self, namespace = None):
        """Create a new completer for the command line.

        Completer([namespace]) -> completer instance.

        If unspecified, the default namespace where completions are performed
        is __main__ (technically, __main__.__dict__). Namespaces should be
        given as dictionaries.
        """

        if namespace and not isinstance(namespace, dict):
            raise TypeError,'namespace must be a dictionary'

        # Don't bind to namespace quite yet, but flag whether the user wants a
        # specific namespace or to use __main__.__dict__. This will allow us
        # to bind to __main__.__dict__ at completion time, not now.
        if namespace is None:
            self.use_main_ns = 1
        else:
            self.use_main_ns = 0
            self.namespace = namespace

    def getLastOf(self, sep, text):
        """ Returns the index of the last occurrence of any char of sep in text
        """
        index = -1
        for c in sep:
            index = max( index, text.rfind(c) )
        return index

    def isInString(self, text, toPos = None):
        """ Returns whether the pos toPos would be inside a string.
            toPos defaults to the end of the string
        """
        if toPos is None:
            toPos = len(text)
        inString = False
        stringEnd = ""
        for c in text[:toPos]:
            if( inString and c == stringEnd ):
                inString = False
            elif((not inString) and (c == "'" or c=='"') ):
                inString = True
                stringEnd = c
        return inString

    def isInFunction(self, funcName, text):
        """ Returns whether we are currently completing inside the
            function with name funcName.
            If that is the case, the return value determines the index
            of the argument we are currently in.
            Otherwise, -1 is returned.
        """
        funcAt = text.rfind( funcName + "(" )
        while funcAt != -1:
            if self.isInString( text, funcAt ):
                funcAt = text.rfind( funcName + "(", 0, funcAt )
                continue
            atArg = 0
            bracketIndex = 0
            for c in text[funcAt + len(funcName) + 1:]:
                if c == "," and bracketIndex == 0:
                    atArg += 1
                elif c == "(":
                    bracketIndex += 1
                elif c == ")":
                    bracketIndex -= 1
            if bracketIndex == 0:
                return atArg
            else:
                funcAt = text.rfind( funcName + "(", 0, funcAt )
                continue

        return -1

    def couldCompleteString(self, text, delim):
        """ Returns whether we would complete a string or an empty
            part after delim
        """
        return self.isInString(text) or text.rsplit(delim, 1)[1].split() == [];

    def getCaller(self, funcName, text):
        """ Returns the object a certain function is called on
        """
        funcAt = text.rfind( funcName + "(" )
        while funcAt != -1:
            if self.isInString( text, funcAt ):
                funcAt = text.rfind( funcName + "(", 0, funcAt )
                continue
            bracketIndex = 0
            for c in text[funcAt + len(funcName) + 1:]:
                if c == "(":
                    bracketIndex += 1
                elif c == ")":
                    bracketIndex -= 1
            if bracketIndex == 0:
                text = text[:funcAt]
                if text[-1] != ".":
                    return None
                text = text[:-1]
                index = self.getLastOf( " (*/[:+-^\t", text )
                if index != -1:
                    text = text[index:]
                try:
                    return eval(text, self.namespace)
                except Exception:
                    return None
            else:
                funcAt = text.rfind( funcName + "(", 0, funcAt )
                continue
        return None

    def getFunctionArgument( self, funcName, arg, text):
        """ Returns the content (string) of the specified argument of the function
        """
        funcAt = text.rfind( funcName + "(" )
        while funcAt != -1:
            if self.isInString( text, funcAt ):
                funcAt = text.rfind( funcName + "(", 0, funcAt )
                continue
            start = funcAt + len(funcName)
            atArg = 0
            bracketIndex = 0
            for pos in range(start + 1, len(text)):
                c = text[pos]
                if c == "," and bracketIndex == 0:
                    if atArg == arg:
                        return text[start + 1:pos]
                    atArg += 1
                    if atArg == arg:
                        start = pos
                elif c == "(":
                    bracketIndex += 1
                elif c == ")":
                    bracketIndex -= 1
            if bracketIndex == 0:
                return text[start + 1:]
            else:
                funcAt = text.rfind( funcName + "(", 0, funcAt )
                continue
        return None

    def completeGetAlgorithm( self, text, typingStart = None):
        """ Completes the getAlgorithm function
        """
        list = self.namespace['fantom'].getAllAlgorithmNames();
        return ['"' + elem + '"' for elem in list if typingStart == None or elem.find(typingStart) == 0]

    def completeMakeAlgorithm( self, text, typingStart = None):
        """ Completes the makeAlgorithm function
        """
        list = self.namespace['fantom'].getAvailableAlgorithms();
        return ['"' + elem + '"' for elem in list if typingStart == None or elem.find(typingStart) == 0]

    def completeOptions( self, algo, typingStart = None):
        """ Completes the option names of algo
        """
        list = algo.getOptionNames();
        return ['"' + elem + '"' for elem in list if typingStart == None or elem.find(typingStart) == 0]

    def completeOutputs( self, algo, typingStart = None):
        """ Completes the option names of algo
        """
        list = algo.getOutputNames();
        return ['"' + elem + '"' for elem in list if typingStart == None or elem.find(typingStart) == 0]

    def complete(self, text, state):
        """Return the next possible completion for 'text'.

        This is called successively with state == 0, 1, 2, ... until it
        returns None.  The completion should begin with 'text'.

        """

        if self.use_main_ns:
            self.namespace = __main__.__dict__

        if state == 0:
            self.prefix = ""
            if self.isInFunction( "fantom.makeAlgorithm", text ) == 0 and self.couldCompleteString(text, "("):
                if self.isInString( text ):
                    startIndex = text.rfind( "(" )
                    index = text.find( "'", startIndex )
                    if text.find( '"', startIndex ) != -1:
                        if index == -1 or text.find( '"', startIndex ) < index:
                            index = text.find( '"', startIndex )
                    self.prefix = text[:index]
                    self.matches = self.completeMakeAlgorithm(text, text[index+1:])
                else:
                    self.prefix = text
                    self.matches = self.completeMakeAlgorithm(text)
            elif self.isInFunction( "fantom.getAlgorithm", text ) == 0 and self.couldCompleteString(text, "("):
                if self.isInString( text ):
                    startIndex = text.rfind( "(" )
                    index = text.find( "'", startIndex )
                    if text.find( '"', startIndex ) != -1:
                        if index == -1 or text.find( '"', startIndex ) < index:
                            index = text.find( '"', startIndex )
                    self.prefix = text[:index]
                    self.matches = self.completeGetAlgorithm(text, text[index+1:])
                else:
                    self.prefix = text
                    self.matches = self.completeGetAlgorithm(text)
            elif self.isInFunction( "getResult", text ) == 0 and self.couldCompleteString(text, "(") and isinstance( self.getCaller( "getResult", text), self.namespace['fantom'].Algorithm ):
                algo = self.getCaller( "getResult", text )
                if self.isInString( text ):
                    startIndex = text.rfind( "(" )
                    index = text.find( "'", startIndex )
                    if text.find( '"', startIndex ) != -1:
                        if index == -1 or text.find( '"', startIndex) < index:
                            index = text.find( '"', startIndex )
                    self.prefix = text[:index]
                    self.matches = self.completeOutputs(algo, text[index+1:])
                else:
                    self.prefix = text
                    self.matches = self.completeOutputs(algo)
            elif self.isInFunction( "setOption", text ) == 0 and self.couldCompleteString(text, "(") and isinstance( self.getCaller( "setOption", text), self.namespace['fantom'].Algorithm ):
                algo = self.getCaller( "setOption", text )
                if self.isInString( text ):
                    startIndex = text.rfind( "(" )
                    index = text.find( "'", startIndex )
                    if text.find( '"', startIndex ) != -1:
                        if index == -1 or text.find( '"', startIndex ) < index:
                            index = text.find( '"', startIndex )
                    self.prefix = text[:index]
                    self.matches = self.completeOptions(algo, text[index+1:])
                else:
                    self.prefix = text
                    self.matches = self.completeOptions(algo)
            elif self.isInFunction( "connect", text ) == 0 and self.couldCompleteString(text, "(") and isinstance( self.getCaller( "connect", text), self.namespace['fantom'].Algorithm ):
                algo = self.getCaller( "connect", text )
                if self.isInString( text ):
                    startIndex = text.rfind( "(" )
                    index = text.find( "'", startIndex )
                    if text.find( '"', startIndex ) != -1:
                        if index == -1 or text.find( '"', startIndex ) < index:
                            index = text.find( '"', startIndex )
                    self.prefix = text[:index]
                    self.matches = self.completeOutputs(algo, text[index+1:])
                else:
                    self.prefix = text
                    self.matches = self.completeOutputs(algo)
            elif self.isInFunction( "connect", text ) == 2 and self.couldCompleteString(text, ",") and isinstance( self.getCaller( "connect", text), self.namespace['fantom'].Algorithm ):
                algo = self.namespace[ self.getFunctionArgument( "connect", 1, text ).strip() ]
                if isinstance( algo, self.namespace['fantom'].Algorithm ):
                    if self.isInString( text ):
                        startIndex = text.rfind( "(" )
                        index = text.find( "'", startIndex )
                        if text.find( '"', startIndex ) != -1:
                            if index == -1 or text.find( '"', startIndex ) < index:
                                index = text.find( '"', startIndex )
                        self.prefix = text[:index]
                        self.matches = self.completeOptions(algo, text[index+1:])
                    else:
                        self.prefix = text
                        self.matches = self.completeOptions(algo)
            else: # ordinary completion
                # first step: split out useless part of text
                splitIndex = self.getLastOf( " (*/[:+-^\t", text )
                if( splitIndex >= 0 ):
                    self.prefix = text[:splitIndex + 1]
                    useFullText = text[splitIndex + 1:]
                else:
                    self.prefix = ""
                    useFullText = text

                if "." in useFullText:
                    self.matches = self.attr_matches(useFullText)
                else:
                    self.matches = self.global_matches(useFullText)

        try:
            return self.prefix + self.matches[state]
        except IndexError:
            return None

    def _callable_postfix(self, val, word):
        if hasattr(val, '__call__'):
            word = word + "("
        return word

    def global_matches(self, text):
        """Compute matches when text is a simple name.

        Return a list of all keywords, built-in functions and names currently
        defined in self.namespace that match.

        """
        import keyword
        matches = []
        n = len(text)
        for word in keyword.kwlist:
            if word[:n] == text:
                matches.append(word)
        for nspace in [__builtin__.__dict__, self.namespace]:
            for word, val in nspace.items():
                if word[:n] == text and word != "__builtins__":
                    matches.append(self._callable_postfix(val, word))
        return matches

    def attr_matches(self, text):
        """Compute matches when text contains a dot.

        Assuming the text is of the form NAME.NAME....[NAME], and is
        evaluable in self.namespace, it will be evaluated and its attributes
        (as revealed by dir()) are used as possible completions.  (For class
        instances, class members are also considered.)

        WARNING: this can still invoke arbitrary C code, if an object
        with a __getattr__ hook is evaluated.

        """
        import re
        m = re.match(r"(\w+(\.\w+)*)\.(\w*)", text)
        if not m:
            return []
        expr, attr = m.group(1, 3)
        try:
            thisobject = eval(expr, self.namespace)
        except Exception:
            return []

        # get the content of the object, except __builtins__
        words = dir(thisobject)
        if "__builtins__" in words:
            words.remove("__builtins__")

        if hasattr(thisobject, '__class__'):
            words.append('__class__')
            words.extend(get_class_members(thisobject.__class__))
        matches = []
        n = len(attr)
        for word in words:
            if word[:n] == attr and hasattr(thisobject, word):
                val = getattr(thisobject, word)
                word = self._callable_postfix(val, "%s.%s" % (expr, word))
                matches.append(word)
        return matches

def get_class_members(klass):
    ret = dir(klass)
    if hasattr(klass,'__bases__'):
        for base in klass.__bases__:
            ret = ret + get_class_members(base)
    return ret
