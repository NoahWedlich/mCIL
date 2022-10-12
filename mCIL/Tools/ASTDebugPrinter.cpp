#include "ASTDebugPrinter.h"

void ASTDebugPrinter::print_expression(expr_ptr expr)
{
	os_ << repr_expr(expr) << std::endl;
}

void ASTDebugPrinter::print_statement(stmt_ptr stmt)
{
	os_ << repr_stmt(stmt) << std::endl;
}

void ASTDebugPrinter::print_expr_list(expr_list exprs)
{
	for (expr_ptr expr : exprs)
	{
		os_ << repr_expr(expr) << std::endl;
	}
}

void ASTDebugPrinter::print_stmt_list(stmt_list stmts)
{
	for (stmt_ptr stmt : stmts)
	{
		os_ << repr_stmt(stmt) << std::endl;
	}
}

std::string ASTDebugPrinter::repr_cil_type(cilType type)
{
	std::string result = " type=";
	switch (type.type)
	{
	case Type::NONE:
		result += "none";
		break;
	case Type::BOOL:
		result += "bool";
		break;
	case Type::NUM:
		result += "num";
		break;
	case Type::STR:
		result += "str";
		break;
	case Type::OBJ:
		result += "obj";
		break;
	case Type::UNKNOWN:
		result += "auto";
		break;
	case Type::ERROR:
		result += "error";
		break;
	}

	result += (type.is_const ? " const=true" : " const=false");
	return result;
}

std::string ASTDebugPrinter::repr_expr(expr_ptr expr)
{
	switch (expr->type())
	{
	case ExprType::EXPRESSION_ERROR:
		return "<ErrorExpression>";
	case ExprType::EXPRESSION_GROUPING:
		return repr_grouping_expr(std::dynamic_pointer_cast<GroupingExpression, Expression>(expr));
	case ExprType::EXPRESSION_CALL:
		return repr_call_expr(std::dynamic_pointer_cast<CallExpression, Expression>(expr));
	case ExprType::EXPRESSION_ACCESS:
		return repr_access_expr(std::dynamic_pointer_cast<AccessExpression, Expression>(expr));
	case ExprType::EXPRESSION_NEW:
		return repr_new_expr(std::dynamic_pointer_cast<NewExpression, Expression>(expr));
	case ExprType::EXPRESSION_ARRAY_ACCESS:
		return repr_array_access_expr(std::dynamic_pointer_cast<ArrayAccessExpression, Expression>(expr));
	case ExprType::EXPRESSION_PRIMARY:
		return repr_primary_expr(std::dynamic_pointer_cast<PrimaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_UNARY:
		return repr_unary_expr(std::dynamic_pointer_cast<UnaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_BINARY:
		return repr_binary_expr(std::dynamic_pointer_cast<BinaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_TERNARY:
		return repr_ternary_expr(std::dynamic_pointer_cast<TernaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_ASSIGNMENT:
		return repr_assignment_expr(std::dynamic_pointer_cast<AssignmentExpression, Expression>(expr));
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of expressions");
	}
}

std::string ASTDebugPrinter::repr_grouping_expr(std::shared_ptr<GroupingExpression> expr)
{
	InitRepr();
	result += "<GroupingExpression>\n";
	ReprChild(repr_expr(expr->expr()));
	return result;
}

std::string ASTDebugPrinter::repr_primary_expr(std::shared_ptr<PrimaryExpression> expr)
{
	InitRepr();
	result += "<PrimaryExpression";
	switch (expr->primary_type())
	{
	case PrimaryType::PRIMARY_BOOL:
		result += " type=bool";
		result += (expr->val().bool_val ? " value=true" : " value=false");
		break;
	case PrimaryType::PRIMARY_NUM:
		result += " type=num";
		result += " value=" + std::to_string(expr->val().num_val);
		break;
	case PrimaryType::PRIMARY_STR:
		result += " type=str";
		result += " value=" + *(expr->val().str_val);
		break;
	case PrimaryType::PRIMARY_IDENTIFIER:
		result += " type=identifier";
		result += " value=" + *(expr->val().identifier_val);
		break;
	}
	result += ">\n";
	return result;
}

std::string ASTDebugPrinter::repr_call_expr(std::shared_ptr<CallExpression> expr)
{
	InitRepr();
	result += "<CallExpression";
	result += " name=" + expr->identifier();
	result += ">\n";
	for (expr_ptr arg : expr->args())
	{
		ReprChild(repr_expr(arg));
	}
	return result;
}

std::string ASTDebugPrinter::repr_access_expr(std::shared_ptr<AccessExpression> expr)
{
	InitRepr();
	result += "<AccessExpression";
	result += " identifier=" + expr->identifier();
	result += ">\n";
	ReprChild(repr_expr(expr->inner()));
	return result;
}

std::string ASTDebugPrinter::repr_new_expr(std::shared_ptr<NewExpression> expr)
{
	InitRepr();
	result += "<NewExpression";
	result += " identifier=" + expr->identifier();
	result += ">\n";
	for (expr_ptr arg : expr->args())
	{
		ReprChild(repr_expr(arg));
	}
	return result;
}

std::string ASTDebugPrinter::repr_array_access_expr(std::shared_ptr<ArrayAccessExpression> expr)
{
	InitRepr();
	result += "<ArrayAccessExpression";
	result += " identifier=" + expr->identifier();
	result += ">\n";
	ReprChild(repr_expr(expr->index()));
	return result;
}

std::string ASTDebugPrinter::repr_unary_expr(std::shared_ptr<UnaryExpression> expr)
{
	InitRepr();
	result += "<UnaryExpression";
	result += " operator=" + TokenPrinter::print_operator(expr->op());
	result += ">\n";
	ReprChild(repr_expr(expr->expr()));
	return result;
}

std::string ASTDebugPrinter::repr_binary_expr(std::shared_ptr<BinaryExpression> expr)
{
	InitRepr();
	result += "<BinaryExpression";
	result += " operator=" + TokenPrinter::print_operator(expr->op());
	result += ">\n";
	ReprChild(repr_expr(expr->left()));
	ReprChild(repr_expr(expr->right()));
	return result;
}

std::string ASTDebugPrinter::repr_ternary_expr(std::shared_ptr<TernaryExpression> expr)
{
	InitRepr();
	result += "<TernaryExpression>\n";
	ReprChild(repr_expr(expr->cond()));
	ReprChild(repr_expr(expr->left()));
	ReprChild(repr_expr(expr->right()));
	return result;
}

std::string ASTDebugPrinter::repr_assignment_expr(std::shared_ptr<AssignmentExpression> expr)
{
	InitRepr();
	result += "<AssignmentExpression>\n";
	ReprChild(repr_expr(expr->target()));
	ReprChild(repr_expr(expr->expr()));
	return result;
}

std::string ASTDebugPrinter::repr_stmt(stmt_ptr stmt)
{
	switch (stmt->type())
	{
	case StmtType::STATEMENT_ERROR:
		return "<ErrorStatment>";
	case StmtType::STATEMENT_BLOCK:
		return repr_block_stmt(std::dynamic_pointer_cast<BlockStatement, Statement>(stmt));
	case StmtType::STATEMENT_BREAK:
		return repr_break_stmt(std::dynamic_pointer_cast<BreakStatement, Statement>(stmt));
	case StmtType::STATEMENT_RETURN:
		return repr_return_stmt(std::dynamic_pointer_cast<ReturnStatement, Statement>(stmt));
	case StmtType::STATEMENT_PRINT:
		return repr_print_stmt(std::dynamic_pointer_cast<PrintStatement, Statement>(stmt));
	case StmtType::STATEMENT_IF:
		return repr_if_stmt(std::dynamic_pointer_cast<IfStatement, Statement>(stmt));
	case StmtType::STATEMENT_WHILE:
		return repr_while_stmt(std::dynamic_pointer_cast<WhileStatement, Statement>(stmt));
	case StmtType::STATEMENT_FOR:
		return repr_for_stmt(std::dynamic_pointer_cast<ForStatement, Statement>(stmt));
	case StmtType::STATEMENT_VAR_DECL:
		return repr_var_decl_stmt(std::dynamic_pointer_cast<VarDeclStatement, Statement>(stmt));
	case StmtType::STATEMENT_ARR_DECL:
		return repr_arr_decl_stmt(std::dynamic_pointer_cast<ArrDeclStatement, Statement>(stmt));
	case StmtType::STATEMENT_FUNC_DECL:
		return repr_func_decl_stmt(std::dynamic_pointer_cast<FuncDeclStatement, Statement>(stmt));
	case StmtType::STATEMENT_CLASS_DECL:
		return repr_class_decl_stmt(std::dynamic_pointer_cast<ClassDeclStatement, Statement>(stmt));
	case StmtType::STATEMENT_EXPR:
		return repr_expr_stmt(std::dynamic_pointer_cast<ExprStatement, Statement>(stmt));
	default:
		throw CILError::error(stmt->pos(), "Incomplete handling of statements");
	}
}

std::string ASTDebugPrinter::repr_block_stmt(std::shared_ptr<BlockStatement> stmt)
{
	InitRepr();
	result += "<BlockStatement>\n";
	for (stmt_ptr child : stmt->inner())
	{
		ReprChild(repr_stmt(child));
	}
	return result;
}

std::string ASTDebugPrinter::repr_break_stmt(std::shared_ptr<BreakStatement> stmt)
{
	InitRepr();
	result += "<BreakStatement>\n";
	return result;
}

std::string ASTDebugPrinter::repr_return_stmt(std::shared_ptr<ReturnStatement> stmt)
{
	InitRepr();
	result += "<ReturnStatement>\n";
	ReprChild(repr_expr(stmt->expr()));
	return result;
}

std::string ASTDebugPrinter::repr_print_stmt(std::shared_ptr<PrintStatement> stmt)
{
	InitRepr();
	result += "<PrintStatement>\n";
	ReprChild(repr_expr(stmt->expr()));
	return result;
}

std::string ASTDebugPrinter::repr_if_stmt(std::shared_ptr<IfStatement> stmt)
{
	InitRepr();
	result += "<IfStatement>\n";
	ReprChild(repr_expr(stmt->cond()));
	ReprChild(repr_stmt(stmt->if_branch()));
	ReprChild(repr_stmt(stmt->else_branch()));
	return result;
}

std::string ASTDebugPrinter::repr_while_stmt(std::shared_ptr<WhileStatement> stmt)
{
	InitRepr();
	result += "<WhileStatement>\n";
	ReprChild(repr_expr(stmt->cond()));
	ReprChild(repr_stmt(stmt->inner()));
	return result;
}

std::string ASTDebugPrinter::repr_for_stmt(std::shared_ptr<ForStatement> stmt)
{
	InitRepr();
	result += "<ForStatement>\n";
	ReprChild(repr_stmt(stmt->init()));
	ReprChild(repr_expr(stmt->cond()));
	ReprChild(repr_expr(stmt->exec()));
	ReprChild(repr_stmt(stmt->inner()));
	return result;
}

std::string ASTDebugPrinter::repr_var_decl_stmt(std::shared_ptr<VarDeclStatement> stmt)
{
	InitRepr();
	result += "<VarDeclStatement";
	result += " name=" + stmt->info().name;
	result += repr_cil_type(stmt->info().type);
	result += ">\n";
	ReprChild(repr_expr(stmt->val()));
	return result;
}

std::string ASTDebugPrinter::repr_arr_decl_stmt(std::shared_ptr<ArrDeclStatement> stmt)
{
	InitRepr();
	result += "<ArrDeclStatement";
	result += " name=" + stmt->info().name;
	result += " size=" + std::to_string(stmt->info().size);
	result += repr_cil_type(stmt->info().type);
	result += ">\n";
	return result;
}

std::string ASTDebugPrinter::repr_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt)
{
	InitRepr();
	result += "<FuncDeclStatement";
	result += " name=" + stmt->info().name;
	result += repr_cil_type(stmt->info().ret_type);
	result += ">\n";
	for (VarInfo arg : stmt->info().args)
	{
		result += std::string(level_, '\t') + "\t<Argument name = " + arg.name + repr_cil_type(arg.type) + ">\n";
	}
	ReprChild(repr_stmt(stmt->body()));
	return result;
}

std::string ASTDebugPrinter::repr_class_decl_stmt(std::shared_ptr<ClassDeclStatement> stmt)
{
	InitRepr();
	result += "<ClassDeclStatement";
	result += " name=" + stmt->info().name;
	result += ">\n";
	for (stmt_ptr method : stmt->methods())
	{
		ReprChild(repr_stmt(method));
	}
	for (stmt_ptr member : stmt->members())
	{
		ReprChild(repr_stmt(member));
	}
	return result;
}

std::string ASTDebugPrinter::repr_expr_stmt(std::shared_ptr<ExprStatement> stmt)
{
	InitRepr();
	result += "<ExprStatement>\n";
	ReprChild(repr_expr(stmt->expr()));
	return result;
}
