# ===== Program structure =====
<program> ::= <type_def_list> <statement_list>
<type_def_list> ::= <type_def> <type_def_list> | epsilon
<type_def> ::= "type" <identifier> "=" "record" <field_list> "end"
<field_list> ::= <field> <field_tail>
<field_tail> ::= <field> <field_tail> | epsilon
<field> ::= <identifier> ":" <type> ";"
<statement_list> ::= <statement> <statement_list> | epsilon
<block> ::= <statement_list>
<statement> ::= <declaration> | <assignment> | <input_stmt> | <read_stmt> | <send_stmt> | <log_stmt> | <wait_stmt> | <if_stmt> | <while_stmt> | <for_stmt> | <event_handler> | <loco_command>

# ===== Types and declarations =====
<simple_type> ::= "int" | "real" | "bool" | "string"
<type> ::= <simple_type> <array_part> | <identifier>
<array_part> ::= "[" <int_literal> "]" | epsilon
<declaration> ::= "var" <identifier> ":" <type> <initializer> ";"
<initializer> ::= "=" <expression> | epsilon
<assignment> ::= <lvalue> "=" <expression> ";"
<lvalue> ::= <identifier> <accessor_list>
<accessor_list> ::= "." <identifier> <accessor_list> | "[" <expression> "]" <accessor_list> | epsilon

# ===== I/O =====
<input_stmt> ::= "input" "(" <lvalue> ")" ";"
<read_stmt> ::= "read" "(" <string_literal> "," <lvalue> ")" ";"
<send_stmt> ::= "send" "(" <string_literal> "," <expression> ")" ";"
<log_stmt> ::= "log" "(" <expression> ")" ";"
<wait_stmt> ::= "wait" "(" <expression> ")" ";"

# ===== Control flow and handlers =====
<if_stmt> ::= "if" <expression> ":" <block> <else_part> "end"
<else_part> ::= "else" ":" <block> | epsilon
<while_stmt> ::= "while" <expression> ":" <block> "end"
<for_stmt> ::= "for" <identifier> "in" <expression> ".." <expression> ":" <block> "end"
<event_handler> ::= "when" <expression> ":" <block> "end"

# ===== Locomotive commands =====
<loco_command> ::= <traction_cmd> | <direction_cmd> | <cruise_cmd> | <brake_cmd> | <emergency_cmd> | <regen_cmd> | <sand_cmd> | <pantograph_cmd> | <breaker_cmd> | <compressor_cmd> | <horn_cmd> | <lights_cmd> | <alarm_cmd> | <PZB_command>
<traction_cmd> ::= "traction" "(" <expression> ")" ";"
<direction_cmd> ::= "direction" "(" <direction_mode> ")" ";"
<direction_mode> ::= "forward" | "reverse" | "neutral"
<cruise_cmd> ::= "cruise" "(" <expression> ")" ";"
<brake_cmd> ::= "brake" "(" <brake_type> "," <brake_position> ")" ";"
<brake_type> ::= "direct" | "primary"
<brake_position> ::= "release" | "running" | "neutral" | "service1" | "service2" | "service3" | "emergency"
<emergency_cmd> ::= "emergency_stop" "(" ")" ";"
<regen_cmd> ::= "regen" "(" <expression> ")" ";"
<sand_cmd> ::= "sand" "(" ")" ";"
<pantograph_cmd> ::= "pantograph" "(" <pantograph_pos> ")" ";"
<pantograph_pos> ::= "up" | "down"
<breaker_cmd> ::= "breaker" "(" <breaker_pos> ")" ";"
<breaker_pos> ::= "open" | "close"
<compressor_cmd> ::= "compressor" "(" <run_mode> ")" ";"
<run_mode> ::= "start" | "stop"
<horn_cmd> ::= "horn" "(" <horn_type> ")" ";"
<horn_type> ::= "high" | "low"
<lights_cmd> ::= "lights" "(" <lights_mode> ")" ";"
<lights_mode> ::= "low" | "high" | "back" | "off"
<alarm_cmd> ::= "alarm" "(" <alarm_type> ")" ";"
<alarm_type> ::= "DSV" | "PZB" | "Fire" | "WheelSlip"
<PZB_command> ::= "PZB" "(" <PZB_mode> ")" ";"
<PZB_mode> ::= "U" | "O" | "M"

# ===== Expressions =====
<expression> ::= <or_expr>
<or_expr> ::= <and_expr> <or_tail>
<or_tail> ::= "||" <and_expr> <or_tail> | epsilon
<and_expr> ::= <not_expr> <and_tail>
<and_tail> ::= "&&" <not_expr> <and_tail> | epsilon
<not_expr> ::= "!" <not_expr> | <relation>
<relation> ::= <additive> <relation_tail>
<relation_tail> ::= <relational_op> <additive> | epsilon
<relational_op> ::= "==" | "!=" | "<" | ">" | "<=" | ">="
<additive> ::= <term> <additive_tail>
<additive_tail> ::= <add_op> <term> <additive_tail> | epsilon
<add_op> ::= "+" | "-"
<term> ::= <factor> <term_tail>
<term_tail> ::= <mul_op> <factor> <term_tail> | epsilon
<mul_op> ::= "*" | "/" | "%"
<factor> ::= "-" <factor> | <primary>
<primary> ::= <int_literal>
            | <real_literal>
            | <string_literal>
            | "true"
            | "false"
            | "alarm" "(" <alarm_type> ")"
            | <identifier> <identifier_suffix>
            | "(" <expression> ")"
<identifier_suffix> ::= "(" <argument_list> ")" | <accessor_list>
<argument_list> ::= <expression> <argument_tail> | epsilon
<argument_tail> ::= "," <expression> <argument_tail> | epsilon

# ===== Lexical rules =====
<identifier> ::= <letter> <identifier_rest>
<identifier_rest> ::= <letter> <identifier_rest> | <digit> <identifier_rest> | "_" <identifier_rest> | epsilon
<int_literal> ::= <digit> <digit_rest>
<digit_rest> ::= <digit> <digit_rest> | epsilon
<real_literal> ::= <int_literal> "." <digit> <digit_rest>
<string_literal> ::= '"' <string_body> '"'
<string_body> ::= <string_char> <string_body> | epsilon
<string_char> ::= <letter> | <digit> | <symbol> | " "
<symbol> ::= "_" | "-" | "+" | "." | "," | ":" | ";" | "!" | "?" | "(" | ")" | "/" | "%" | "#" | "=" | "<" | ">"
<letter> ::= "a" | "b" | ... | "z" | "A" | "B" | ... | "Z"
<digit> ::= "0" | "1" | ... | "9"
