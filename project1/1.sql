SELECT name, Count(owner_id) AS Caught FROM Trainer AS t, CatchedPokemon AS cp
WHERE t.id = cp.owner_id GROUP BY name HAVING caught>=3 ORDER BY caught;
