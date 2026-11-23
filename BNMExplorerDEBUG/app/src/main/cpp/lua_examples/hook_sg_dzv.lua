local hook_id = hook_method("sg", "dzv", 2, function(self, hashSet, camera)
    print("[sg.dzv] intercepted; original skipped")
    return false
end)

print("[sg.dzv] hook installed, id =", hook_id)
