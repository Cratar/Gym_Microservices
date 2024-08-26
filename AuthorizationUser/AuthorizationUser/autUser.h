#pragma once
#include "crow.h"


#ifndef AUTUSER_H
#define AUTUSER_H

bool AuthorizationUser(const crow::json::rvalue& email, const crow::json::rvalue& password);


#endif