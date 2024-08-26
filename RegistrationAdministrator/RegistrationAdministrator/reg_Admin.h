#pragma once


#pragma once
#include <string>
#include "crow.h"

#ifndef REG_USER_H
#define REG_USER_H

bool RegistrationAdministrator(const crow::json::rvalue& name, const crow::json::rvalue& surname, const crow::json::rvalue& email, const crow::json::rvalue& post, const crow::json::rvalue& age, const crow::json::rvalue& password);

#endif
