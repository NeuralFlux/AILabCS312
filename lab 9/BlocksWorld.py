# Parse the string predicates
def parse_pred(string):
    pred = string[1:-1].split()

    if pred[0] == "on":
        return Predicate(1, pred[1], pred[2])
    elif pred[0] == "ontable":
        return Predicate(2, pred[1])
    elif pred[0] == "hold":
        return Predicate(3, pred[1])
    elif pred[0] == "clear":
        return Predicate(4, pred[1])
    elif pred[0] == "AE":
        return Predicate(5)
    else:
        return ValueError


class BlocksWorld(object):
    def __init__(self):

        # Initialise World
        self.plan = []
        self.stack = []
        self.state = State()

        # Initialise with Arm Empty
        self.state.predicates.add(Predicate(5))

    def read_inputs(self, file_name):
        start, goal_state = [], []

        with open(file_name, 'r') as file:
            index = 0
            for line in file:
                if index == 0:
                    pass

                elif index == 1:
                    line = line.replace('\n', '')
                    preds = line.split('^')

                    for pred in preds:
                        start.append(parse_pred(pred))

                elif index == 2:
                    line = line.replace('\n', '')
                    preds = line.split('^')

                    for pred in preds:
                        goal_state.append(parse_pred(pred))

                index += 1

        self.initialise_states(start)

        return goal_state

    def initialise_states(self, start):
        on = {}

        for pred in start:
            # Add predicate to state
            self.state.predicates.add(pred)

            if pred.p_type != 5:
                # To keep track of clear()
                if pred.op1 not in on.keys():
                    on[pred.op1] = False

                # Add clear if nothing on block
                if on[pred.op1] is False:
                    self.state.predicates.add(Predicate(4, pred.op1))

                # If hold(a), remove AE
                if pred.p_type == 3:
                    self.state.predicates.difference_update(set([Predicate(5)]))

                # Remove clear if something on block
                if pred.p_type == 1:
                    on[pred.op2] = True
                    self.state.predicates.difference_update(set([Predicate(4, pred.op2)]))

    def relevant_action(self, pred):

        # If on(a,b) return stack(a,b)
        if pred.p_type == 1:
            return Action(1, pred.op1, pred.op2)

        # If ontable(a) return putdown(a)
        elif pred.p_type == 2:
            return Action(4, pred.op1)

        # If hold(a)
        elif pred.p_type == 3:

            # If ontable(a), return pick(a)
            if Predicate(2, pred.op1) in self.state.predicates:
                return Action(3, pred.op1)

            # Otherwise, unstack(a, ?)
            else:
                for test_p in self.state.predicates:
                    if test_p.p_type == 1 and test_p.op1 == pred.op1:
                        return Action(2, pred.op1, test_p.op2)

        # If clear(a)
        elif pred.p_type == 4:

            # If hold(a), return putdown(a)
            if Predicate(3, pred.op1) in self.state.predicates:
                return Action(4, pred.op1)

            # Otherwise, unstack(?, a)
            else:
                for test_p in self.state.predicates:
                    if test_p.p_type == 1 and test_p.op2 == pred.op1:
                        return Action(2, test_p.op1, test_p.op2)

        # If AE, return putdown(a)
        elif pred.p_type == 5:
            for test_p in self.state.predicates:
                if test_p.p_type == 3:
                    return Action(4, test_p.op1)

        else:
            raise TypeError

    def goal_stack_planning(self, goal_predicates):

        # Add goal and its predicates to stack
        self.stack.append(goal_predicates)
        for pred in goal_predicates[::-1]:
            self.stack.append(pred)

        # Go on till stack is empty
        while len(self.stack) != 0:

            # Pop last item
            x = self.stack.pop()

            # Act according to x's nature
            if x.__class__.__name__ == 'list':  # conjunction of predicates

                # Check if predicates hold
                if self.state.check_predicate(x) is True:
                    continue
                else:
                    # add full literals and itself
                    self.stack.append(x)
                    for pred in x[::-1]:
                        self.stack.append(pred)

            elif x.__class__.__name__ == 'Predicate':  # just one predicate

                # Check if predicate holds
                if self.state.check_predicate(set([x])) is True:
                    continue
                else:
                    # Choose a relevant action
                    rel_act = self.relevant_action(x)

                    # Add action and preconds to stack from behind
                    self.stack.append(rel_act)
                    self.stack.append(rel_act.preconds)
                    for cond in rel_act.preconds[::-1]:
                        self.stack.append(cond)

            elif x.__class__.__name__ == 'Action':  # an action

                # Execute action and add it to the plan
                self.state.progress(x)
                self.plan.append(x)
            else:
                raise TypeError

        # # Print plan when done
        # for action in self.plan:
        #     print(action)

    def write_to_file(self, file):
        for action in self.plan:
            file.write(str(action) + '\n')


class State(object):
    def __init__(self):
        self.predicates = set()

    def check_predicate(self, predicates):
        return self.predicates.issuperset(predicates)

    def progress(self, action):
        self.predicates.difference_update(action.neg_effects)
        self.predicates.update(action.pos_effects)


class Action(object):

    translate = {1: "stack", 2: "unstack", 3: "pick", 4: "putdown"}

    def __init__(self, a_type, op1, op2=None):

        # Verify predicate
        if a_type == 1:
            assert op1 is not None and op2 is not None
        elif a_type == 2:
            assert op1 is not None and op2 is not None
        elif a_type == 3:
            assert op1 is not None and op2 is None
        elif a_type == 4:
            assert op1 is not None and op2 is None
        else:
            raise ValueError

        # Store action
        self.a_type = a_type
        self.op1 = op1
        self.op2 = op2

        # Formulate preconds and effects
        # Note - Push preconds and then add them from behind
        # to the stack. Hence whatever is first will be on top of stack.
        self.preconds = list()
        self.pos_effects = set()
        self.neg_effects = set()

        if a_type == 1:
            self.preconds.append(Predicate(4, op1))
            self.preconds.append(Predicate(4, op2))
            self.preconds.append(Predicate(5))

            self.pos_effects.add(Predicate(5))
            self.pos_effects.add(Predicate(1, op1, op2))
            self.pos_effects.add(Predicate(4, op1))

            self.neg_effects.add(Predicate(4, op2))
            self.neg_effects.add(Predicate(3, op1))
        elif a_type == 2:
            self.preconds.append(Predicate(1, op1, op2))
            self.preconds.append(Predicate(4, op1))
            self.preconds.append(Predicate(5))

            self.pos_effects.add(Predicate(4, op2))
            self.pos_effects.add(Predicate(3, op1))

            self.neg_effects.add(Predicate(4, op1))
            self.neg_effects.add(Predicate(5))
            self.neg_effects.add(Predicate(1, op1, op2))
        elif a_type == 3:
            self.preconds.append(Predicate(2, op1))
            self.preconds.append(Predicate(4, op1))
            self.preconds.append(Predicate(5))

            self.pos_effects.add(Predicate(3, op1))

            self.neg_effects.add(Predicate(5))
            self.neg_effects.add(Predicate(4, op1))
            self.neg_effects.add(Predicate(2, op1))
        elif a_type == 4:
            self.preconds.append(Predicate(3, op1))

            self.pos_effects.add(Predicate(4, op1))
            self.pos_effects.add(Predicate(5))
            self.pos_effects.add(Predicate(2, op1))

            self.neg_effects.add(Predicate(3, op1))
        else:
            raise ValueError

    def __str__(self):
        return "(" + Action.translate[self.a_type] + " " + self.op1 + ")" if self.op2 is None \
            else "(" + Action.translate[self.a_type] + " " + self.op1 + " " + self.op2 + ")"


class Predicate(object):

    translate = {1: "on", 2: "ontable", 3: "hold", 4: "clear", 5: "AE"}

    def __init__(self, p_type, op1=None, op2=None):

        # Verify predicate
        if p_type == 1:
            assert op1 is not None and op2 is not None
        elif p_type == 2:
            assert op1 is not None and op2 is None
        elif p_type == 3:
            assert op1 is not None and op2 is None
        elif p_type == 4:
            assert op1 is not None and op2 is None
        elif p_type == 5:
            assert op1 is None and op2 is None
        else:
            raise ValueError

        # Store predicate
        self.p_type = p_type
        self.op1 = op1
        self.op2 = op2

    def __eq__(self, value):
        return self.p_type == value.p_type and self.op1 == value.op1 and \
            self.op2 == value.op2

    def __repr__(self):
        if self.op1 is not None and self.op2 is not None:
            return "(" + Predicate.translate[self.p_type] + " " + self.op1 + " " + self.op2 + ")"
        elif self.op1 is not None:
            return "(" + Predicate.translate[self.p_type] + " " + self.op1 + ")"
        else:
            return "(" + Predicate.translate[self.p_type] + ")"

    def __hash__(self):
        return hash(repr(self))
