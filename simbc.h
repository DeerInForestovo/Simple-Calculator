enum errors_in_bc {
    no_error,
    syntax_error,
    invalid_variable_name,
    runtime_error,
    unknown_command,
    unknown_variable,
    unknown_function
};
struct error_in_bc {
    errors_in_bc error_type;
    char* pos;
    error_in_bc(errors_in_bc e = no_error, char* p = 0) {
        error_type = e;
        pos = p;
    }
};