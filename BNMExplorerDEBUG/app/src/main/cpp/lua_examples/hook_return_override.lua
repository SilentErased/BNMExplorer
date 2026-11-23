local hook_id = hook_method("sg", "eqp", 0, function(self)
    print("[sg.eqp] returning null Camera")
    return HOOK_NULL
end)

print("[sg.eqp] hook installed, id =", hook_id)
