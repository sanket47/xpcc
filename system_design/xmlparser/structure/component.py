#!/usr/bin/env python
# -*- coding: utf-8 -*-

import helper
from exception import ParserError
from component_element import Action, Event, EventContainer

class Component(object):
	"""
	Representation of a component.
	"""
	def __init__(self, name, reference=False):
		self.name = name
		self.description = None
		
		self.id = None
		self.extends = None
		
		self.reference = reference
		self.abstract = True
		
		self.actions = helper.SingleAssignDict("action")
		self.events = EventContainer()
	
	def check(self):
		# check own values
		helper.check_name_notation(self, self.name)
		
		# check the action, attributes and events
		for list in [self.actions, self.events.publish, self.events.subscribe]:
			for element in list:
				element.check()
	
	def extend(self, top):
		self.extends = top.extends
		
		# update the attributes if they are not None
		self.description = self.description or top.description
		self.id = self.id or top.id
		
		# update actions and events
		self.actions.update(top.actions)
		self.events.update(top.events)
	
	def _from_xml(self, node):
		self.description = helper.xml_read_description(node)
		self.id = helper.xml_read_identifier(node)
		
		self.extends = node.get('extends')
		
		# parse functions and events
		self.__parse_actions(node)
		self.__parse_events(node)
		
	def __parse_actions(self, node):
		node = node.find('actions')
		if node == None:
			return
		
		for subnode in node.findall('action'):
			element = Action(name = subnode.get('name'), 
							reference = self.reference)
			element._from_xml(subnode)
			self.actions[element.name] = element
	
	def __parse_events(self, node):
		# parse events
		eventsNode = node.find('events')
		if eventsNode == None:
			return
		
		publishNode = eventsNode.find('publish')
		if publishNode:
			self.__parse_event(publishNode, self.events.publish)
		
		subscribeNode = eventsNode.find('subscribe')
		if subscribeNode:
			self.__parse_event(subscribeNode, self.events.subscribe)
	
	def __parse_event(self, node, list):
		for subnode in node.findall('event'):
			element = Event(name = subnode.get('name'), 
							reference = self.reference)
			element._from_xml(subnode)
			list[element.name] = element
	
	def __cmp__(self, other):
		return cmp(self.id, other.id) or cmp(self.name, other.name)
	
	def __str__(self):
		"""
		Print a nice UML-like box of content of the component
		
		Example:
		/================================================\
		|               Stage Object [ee]                |
		|------------------------------------------------|
		| actions:                                       |
		| +[00] ping()                                   |
		| +[01] reset()                                  |
		| +[03] save_configuration()                     |
		|------------------------------------------------|
		| publish:                                       |
		| +[02] error : Error Payload                    |
		| +[0f] debug_message : char[]                   |
		|------------------------------------------------|
		| subscribe:                                     |
		| +[02] error : Error Payload                    |
		\================================================/
		
		The numbers in the square brackets shows the identifier (in
		hexadecimal). First block shows the actions and the second the events.
		
		"""
		name = "%s [%02x]" % (self.name, self.id)
		max_length = len(name)
		
		elements = [["actions:"], ["publish:"], ["subscribe:"]]
		for element in elements:
			max_length = max(max_length, len(element[0]))
		
		for i, iterator in enumerate([self.actions, self.events.publish, self.events.subscribe]):
			for element in iterator.iter(reference=None):
				string = "+%s" % element
				max_length = max(max_length, len(string))
				elements[i].append(string)
		
		output  = "/=" + "=" * max_length + "=\\\n"
		
		length = (max_length - len(name)) / 2.0
		pre = " " * int(length)
		post = pre + (" " if (length != int(length)) else "")
		output += "| " + pre + name + post + " |\n"
		
		for element in elements:
			output += "|-" + "-" * max_length + "-|\n"
			for string in element:
				length = max_length - len(string)
				output += "| " + string + " " * length + " |\n"
		
		output += "\\=" + "=" * max_length + "=/\n"
		return output[:-1]


# -----------------------------------------------------------------------------
if __name__ == '__main__':
	component = Component("test1")
	
	component2 = Component("test2")
	component2.add_action(Action("action1"))
	
	component.extend(component2)
	
	print component
	
	action_top = Action("action", returnType="returnType")
	
	action = Action("action")
	action.update(action_top)
	
	print action
	print action.__dict__
