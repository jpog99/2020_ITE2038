SELECT name,max(s.total)
FROM (SELECT SUM(level) AS total,name
      FROM CatchedPokemon, Trainer AS t
      WHERE owner_id=t.id
      GROUP BY owner_id) AS s;
